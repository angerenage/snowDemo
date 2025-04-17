#include "tree.h"

#define TREE_CHUNK_NBR 10
#define TREE_CHUNK_SIZE 10.0f

typedef struct geometry_s {
	vec3* vertices;
	vec3* normals;
	unsigned int vertexCount;
	unsigned int* indices;
	unsigned int indexCount;
} Geometry;

typedef struct point_s {
	vec3 position;
	float radius;
} Point;

static const float treeScale = 0.7f;
static Mesh treeMeshs[TREE_CHUNK_NBR];
static InstancedMesh treeInstances[TREE_CHUNK_NBR];
static mat4 treeModels[TREE_CHUNK_NBR];

static Geometry generateSplineMesh(const Point *points, int numPoints, float generalRadius, int resolution, const vec3 *up) {
	int totalVertices = numPoints * resolution;
	vec3 *vertices = (vec3*)malloc(totalVertices * sizeof(vec3));
	vec3 *normals = (vec3*)malloc(totalVertices * sizeof(vec3));

	vec3 dir = vec3_normalize(vec3_sub(points[1].position, points[0].position));

	for (int i = 0; i < numPoints; i++) {
		Point p = points[i];
		float radius = p.radius * generalRadius;

		vec3 right = vec3_cross(dir, *up);

		for (int j = 0; j < resolution; j++) {
			float angle = (2.0f * (float)M_PI * j) / (float)resolution;
			float cosA = cosf(angle);
			float sinA = sinf(angle);

			vec3 normal = vec3_normalize(vec3_add(vec3_scale(right, cosA), vec3_scale(*up, sinA)));
			normals[i * resolution + j] = normal;

			vertices[i * resolution + j].x = p.position.x + radius * (cosA * right.x + sinA * up->x);
			vertices[i * resolution + j].y = p.position.y + radius * (cosA * right.y + sinA * up->y);
			vertices[i * resolution + j].z = p.position.z + radius * (cosA * right.z + sinA * up->z);
		}

		if (i < numPoints - 1) {
			dir = vec3_normalize(vec3_sub(points[i + 1].position, points[i].position));
		}
	}

	int totalIndices = (numPoints - 1) * resolution * 6;
	unsigned int *indices = (unsigned int*)malloc(totalIndices * sizeof(unsigned int));

	int index = 0;

	for (int i = 0; i < numPoints - 1; i++) {
		for (int j = 0; j < resolution; j++) {
			unsigned int nextJ = (j + 1) % resolution;
			unsigned int current = i * resolution + j;
			unsigned int next = i * resolution + nextJ;
			unsigned int nextRow = (i + 1) * resolution + j;
			unsigned int nextRowNext = (i + 1) * resolution + nextJ;

			indices[index++] = next;
			indices[index++] = current;
			indices[index++] = nextRow;

			indices[index++] = nextRowNext;
			indices[index++] = next;
			indices[index++] = nextRow;
		}
	}

	return (Geometry) {
		.vertices = vertices,
		.normals = normals,
		.vertexCount = totalVertices,
		.indices = indices,
		.indexCount = totalIndices
	};
}

static Geometry mergeGeometries(Geometry *geometries, int count) {
	unsigned int totalVertices = 0, totalIndices = 0;

	for (int i = 0; i < count; i++) {
		totalVertices += geometries[i].vertexCount;
		totalIndices += geometries[i].indexCount;
	}

	vec3 *mergedVertices = (vec3*)malloc(totalVertices * sizeof(vec3));
	vec3 *mergedNormals  = (vec3*)malloc(totalVertices * sizeof(vec3));
	unsigned int *mergedIndices = (unsigned int*)malloc(totalIndices * sizeof(unsigned int));

	unsigned int vertexOffset = 0, indexOffset = 0;

	for (int i = 0; i < count; i++) {
		Geometry g = geometries[i];

		for (unsigned int j = 0; j < g.vertexCount; j++) {
			mergedVertices[vertexOffset + j] = g.vertices[j];
			mergedNormals[vertexOffset + j] = g.normals[j];
		}

		for (unsigned int j = 0; j < g.indexCount; j++) {
			mergedIndices[indexOffset + j] = g.indices[j] + vertexOffset;
		}

		vertexOffset += g.vertexCount;
		indexOffset += g.indexCount;
	}

	return (Geometry) {
		.vertices = mergedVertices,
		.normals = mergedNormals,
		.vertexCount = totalVertices,
		.indices = mergedIndices,
		.indexCount = totalIndices
	};
}

static void freeGeometry(Geometry g) {
	free(g.vertices);
	free(g.normals);
	free(g.indices);
}

static float taper(float t, float length) {
	float linearTerm = 3.0f - (3.0f * t);
	float logTerm = 1.0f - log1pf(t * length) / log1pf(length * 2);
	return fmaxf(0.0f, smoothMin(linearTerm, logTerm, 7.0f));
}

static float branchLength(float t, float baseBranchLength) {
	if (t <= 0.05f) return baseBranchLength * (0.8f + t * 5.0f);
	else return baseBranchLength * (1.2f * (1.0f - t)) + 0.3f;
}

static float gravityEffect(float x, float length) {
	return (-expf(-2.0f * x) + 1.0f) * length * 0.2f;
}

static float growthForce(float t) {
	return 0.4f * t;
}

static Geometry generateBranch(Point* trunkPoints, int trunkSegments, float trunkHeight, float t, float rotationAngle, float baseBranchLength, int segments) {
	Point* points = (Point*)malloc((segments + 1) * sizeof(Point));
	float branchLengthValue = branchLength(t, baseBranchLength);

	float firstPointY = trunkPoints[1].position.y;
	float lastPointY = trunkPoints[trunkSegments - 2].position.y + firstPointY / 3.0f;
	firstPointY += firstPointY / 3.0f;

	float attachHeight = firstPointY + (lastPointY - firstPointY) * t;
	float h = attachHeight / trunkHeight;
	int pointIndex = (int)(h * (trunkSegments - 1));
	float factor = (h * (trunkSegments - 1)) - pointIndex;

	vec3 trunkPos = {
		trunkPoints[pointIndex].position.x * (1 - factor) + trunkPoints[pointIndex + 1].position.x * factor,
		trunkPoints[pointIndex].position.y * (1 - factor) + trunkPoints[pointIndex + 1].position.y * factor,
		trunkPoints[pointIndex].position.z * (1 - factor) + trunkPoints[pointIndex + 1].position.z * factor
	};

	float randomOffsetZ = 0.0f;

	for (int i = 0; i <= segments; i++) {
		float x = (float)i / segments;
		float radius = taper(x, branchLengthValue) * 0.1f;

		float gravity = gravityEffect(x, branchLengthValue);
		float growth = growthForce(t) * x;

		float randomOffsetX = randomFloat(-0.01f, 0.01f) * branchLengthValue * (1.0f - x);
		randomOffsetZ += randomFloat(-0.01f, 0.01f) * branchLengthValue * (1.0f - x);
		float randomOffsetY = randomFloat(-0.01f, 0.01f) * branchLengthValue * (1.0f - x);

		float endX = trunkPos.x + x * branchLengthValue + randomOffsetX;
		float endY = trunkPos.y - gravity + growth + randomOffsetY;
		float endZ = trunkPos.z + randomOffsetZ;

		float rotatedX = cosf(rotationAngle) * (endX - trunkPos.x) - sinf(rotationAngle) * (endZ - trunkPos.z) + trunkPos.x;
		float rotatedZ = sinf(rotationAngle) * (endX - trunkPos.x) + cosf(rotationAngle) * (endZ - trunkPos.z) + trunkPos.z;

		points[i].position = (vec3){rotatedX, endY, rotatedZ};
		points[i].radius = radius;
	}

	float radius = (2.0f - t) / 2.0f * fminf(1.0f, (1.0f - t) + 0.8f);

	Geometry branch = generateSplineMesh(points, segments + 1, radius, 8, &(vec3){0.0f, 1.0f, 0.0f});
	free(points);
	return branch;
}

static Geometry generateTrunk(float height, float scaleFactor, int segments, int numBranches, float baseBranchLength) {
	Point* points = (Point*)malloc(++segments * sizeof(Point));
	float lastX = 0, lastZ = 0;

	const float randomness = 0.3f;

	for (int i = 0; i < segments; i++) {
		float t = (float)i / (segments - 1);
		float heightPos = height * t;
		float radius = taper(t, height);

		float angle = randomFloat(0.0f, 2.0f * (float)M_PI);
		float offsetX = cosf(angle) * t * randomness * radius;
		float offsetZ = sinf(angle) * t * randomness * radius;
		lastX += offsetX;
		lastZ += offsetZ;

		points[i].position = (vec3){lastX, heightPos, lastZ};
		points[i].radius = radius;
	}

	Geometry trunk = generateSplineMesh(points, segments, scaleFactor, 16, &(vec3){1.0f, 0.0f, 0.0f});
	Geometry* branches = (Geometry*)malloc(numBranches * sizeof(Geometry));

	float rotationAngle = randomFloat(0.0f, 2.0f * (float)M_PI);

	for (int i = 0; i < numBranches; i++) {
		float t = powf((float)i / (numBranches - 1), 0.8f) + randomFloat(-0.01f, 0.01f);
		rotationAngle += ((float)M_PI / 3.0f) + randomFloat(-(float)M_PI / 6.0f, (float)M_PI / 6.0f);
		branches[i] = generateBranch(points, segments, height, t, rotationAngle, baseBranchLength, 5);
	}

	Geometry tree = mergeGeometries(branches, numBranches);
	tree = mergeGeometries((Geometry[]){trunk, tree}, 2);

	for (int i = 0; i < numBranches; i++) {
		freeGeometry(branches[i]);
	}
	free(branches);
	free(points);
	freeGeometry(trunk);

	return tree;
}

static Mesh meshFromGeometry(Geometry g) {
	GLuint VAO, VBO[2], EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(2, VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// Positions
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, g.vertexCount * sizeof(vec3), g.vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	// Normales
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, g.vertexCount * sizeof(vec3), g.normals, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	// Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, g.indexCount * sizeof(unsigned int), g.indices, GL_STATIC_DRAW);

	glBindVertexArray(0);

	return (Mesh) {
		.VAO = VAO,
		.vertexCount = g.vertexCount,
		.indexCount = g.indexCount
	};
}

static Mesh generateTree(float height, float scaleFactor, int segments, int numBranches, float baseBranchLength) {
	Geometry treeGeometry = generateTrunk(height, scaleFactor, segments, numBranches, baseBranchLength);
	Mesh treeMesh = meshFromGeometry(treeGeometry);
	freeGeometry(treeGeometry);
	return treeMesh;
}

static mat4* generateTreeInstances(int rows, int cols, float spacing) {
	int count = rows * cols;
	mat4* instances = malloc(sizeof(mat4) * count);
	if (!instances) return NULL;

	int index = 0;
	for(int i = 0; i < rows; i++) {
		for(int j = 0; j < cols; j++) {
			float x = j * spacing - (cols * spacing) / 2.0f;
			float z = i * spacing - (rows * spacing) / 2.0f;
			float y = 0.0f;

			x += randomFloat(-spacing / 2.0f, spacing / 2.0f);
			z += randomFloat(-spacing / 2.0f, spacing / 2.0f);

			float scale = 0.8f + (rand() % 40) / 100.0f;
			float angle = (float)(rand() % 360);

			instances[index++] = transformMatrix((vec3){x, y, z}, (vec3){0.0f, angle * ((float)M_PI / 180.0f), 0.0f}, (vec3){scale, scale, scale});
		}
	}
	return instances;
}

static InstancedMesh bindTreeInstances(const Mesh* tree, const mat4* instances, int instanceCount) {
	GLuint instanceVBO;
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, instanceCount * sizeof(mat4), instances, GL_STATIC_DRAW);

	glBindVertexArray(tree->VAO);
	for(int i = 0; i < 4; i++) {
		glEnableVertexAttribArray(3 + i);
		glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(vec4) * i));
		glVertexAttribDivisor(3 + i, 1);
	}
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return (InstancedMesh) {
		.VAO = tree->VAO,
		.instanceVBO = instanceVBO,
		.vertexCount = tree->vertexCount,
		.indexCount = tree->indexCount,
		.instanceCount = instanceCount
	};
}

static void renderTreeChunk(GLuint shader, const InstancedMesh* trees, const mat4* projection, const mat4* view, const mat4* model, const vec3* lightPos) {
	glUseProgram(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, (GLfloat*)projection);
	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, (GLfloat*)view);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (GLfloat*)model);
	glUniform3fv(glGetUniformLocation(shader, "lightPos"), 1, (GLfloat*)lightPos);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowMap);

	glUniform1i(glGetUniformLocation(shader, "shadowMap"), 0);
	glUniformMatrix4fv(glGetUniformLocation(shader, "shadowProjection"), 1, GL_FALSE, (GLfloat*)&shadowProjection);
	glUniformMatrix4fv(glGetUniformLocation(shader, "shadowView"), 1, GL_FALSE, (GLfloat*)&shadowView);

	glBindVertexArray(trees->VAO);
	glDrawElementsInstanced(GL_TRIANGLES, trees->indexCount, GL_UNSIGNED_INT, 0, trees->instanceCount);
}

void initTrees() {
	for (int i = 0; i < TREE_CHUNK_NBR; i++) {
		Mesh tree = generateTree(10.0f, 0.4f, 10, 100, 3.0f);
		treeMeshs[i] = tree;
		treeInstances[i] = bindTreeInstances(&tree, generateTreeInstances(10, 10, 3.0f), 50);
		treeModels[i] = transformMatrix((vec3){15.0f, 0.1f, i * TREE_CHUNK_SIZE}, (vec3){0.0f, 0.0f, 0.0f}, (vec3){treeScale, treeScale, treeScale});
	}
}

void renderTrees(GLuint shader, const mat4* projection, const mat4* view, const vec3* lightPos, int chunkZ) {
	for (int i = chunkZ; i < TREE_CHUNK_NBR; i++) {
		renderTreeChunk(shader, &treeInstances[i], projection, view, &treeModels[i], lightPos);
	}
}

void cleanupTrees() {
	for (int i = 0; i < TREE_CHUNK_NBR; i++) {
		freeMesh(treeMeshs[i]);
		freeInstancedMesh(treeInstances[i]);
	}
}
