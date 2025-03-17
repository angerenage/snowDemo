#include "tree.h"

typedef struct geometry_s {
	vec3* vertices;
	unsigned int vertexCount;
	unsigned int* indices;
	unsigned int indexCount;
} Geometry;

typedef struct point_s {
	vec3 position;
	float radius;
} Point;

static Geometry generateSplineMesh(const Point *points, int numPoints, float generalRadius, int resolution, const vec3 *up) {
	int totalVertices = numPoints * resolution;
	vec3 *vertices = (vec3*)malloc(totalVertices * sizeof(vec3));

	vec3 dir = vec3_normalize(vec3_sub(points[1].position, points[0].position));

	for (int i = 0; i < numPoints; i++) {
		Point p = points[i];
		float radius = p.radius * generalRadius;

		vec3 right = vec3_cross(dir, *up);

		for (int j = 0; j < resolution; j++) {
			float angle = (2.0f * M_PI * j) / resolution;
			float cosA = cos(angle);
			float sinA = sin(angle);

			vertices[i * resolution + j].x = p.position.x + radius * (cosA * right.x + sinA * up->x);
			vertices[i * resolution + j].y = p.position.y + radius * (cosA * right.y + sinA * up->y);
			vertices[i * resolution + j].z = p.position.z + radius * (cosA * right.z + sinA * up->z);
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
	unsigned int *mergedIndices = (unsigned int*)malloc(totalIndices * sizeof(unsigned int));

	unsigned int vertexOffset = 0, indexOffset = 0;

	for (int i = 0; i < count; i++) {
		Geometry g = geometries[i];

		for (unsigned int j = 0; j < g.vertexCount; j++) {
			mergedVertices[vertexOffset + j] = g.vertices[j];
		}

		for (unsigned int j = 0; j < g.indexCount; j++) {
			mergedIndices[indexOffset + j] = g.indices[j] + vertexOffset;
		}

		vertexOffset += g.vertexCount;
		indexOffset += g.indexCount;
	}

	return (Geometry) {
		.vertices = mergedVertices,
		.vertexCount = totalVertices,
		.indices = mergedIndices,
		.indexCount = totalIndices
	};
}

static void freeGeometry(Geometry g) {
	free(g.vertices);
	free(g.indices);
}

static float taper(float t, float length) {
	float linearTerm = 3.0f - (3.0f * t);
	float logTerm = 1.0f - log1pf(t * length) / log1pf(length * 2);
	return fmax(0.0f, smoothMin(linearTerm, logTerm, 7.0f));
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

Geometry generateBranch(Point* trunkPoints, int trunkSegments, float trunkHeight, float t, float rotationAngle, float baseBranchLength, int segments) {
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

		float rotatedX = cos(rotationAngle) * (endX - trunkPos.x) - sin(rotationAngle) * (endZ - trunkPos.z) + trunkPos.x;
		float rotatedZ = sin(rotationAngle) * (endX - trunkPos.x) + cos(rotationAngle) * (endZ - trunkPos.z) + trunkPos.z;

		points[i].position = (vec3){rotatedX, endY, rotatedZ};
		points[i].radius = radius;
	}

	float radius = (2.0f - t) / 2.0f * fmin(1.0f, (1.0f - t) + 0.8f);

	Geometry branch = generateSplineMesh(points, segments + 1, radius, 8, &(vec3){0.0f, 1.0f, 0.0f});
	free(points);
	return branch;
}

Geometry generateTrunk(float height, float scaleFactor, int segments, int numBranches, float baseBranchLength) {
	Point* points = (Point*)malloc(++segments * sizeof(Point));
	float lastX = 0, lastZ = 0;

	const float randomness = 0.3f;

	for (int i = 0; i < segments; i++) {
		float t = (float)i / (segments - 1);
		float heightPos = height * t;
		float radius = taper(t, height);

		float angle = randomFloat(0, 2 * M_PI);
		float offsetX = cos(angle) * t * randomness * radius;
		float offsetZ = sin(angle) * t * randomness * radius;
		lastX += offsetX;
		lastZ += offsetZ;

		points[i].position = (vec3){lastX, heightPos, lastZ};
		points[i].radius = radius;
	}

	Geometry trunk = generateSplineMesh(points, segments, scaleFactor, 16, &(vec3){1.0f, 0.0f, 0.0f});
	Geometry* branches = (Geometry*)malloc(numBranches * sizeof(Geometry));

	float rotationAngle = randomFloat(0, 2 * M_PI);

	for (int i = 0; i < numBranches; i++) {
		float t = powf((float)i / (numBranches - 1), 0.8f) + randomFloat(-0.01f, 0.01f);
		rotationAngle += (M_PI / 3) + randomFloat(-M_PI / 6, M_PI / 6);
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
	GLuint VAO = createIndexedVAO(g.vertices, g.vertexCount, g.indices, g.indexCount);

	return (Mesh) {
		.VAO = VAO,
		.vertexCount = g.vertexCount,
		.indexCount = g.indexCount
	};
}

Mesh generateTree(float height, float scaleFactor, int segments, int numBranches, float baseBranchLength) {
	Geometry treeGeometry = generateTrunk(height, scaleFactor, segments, numBranches, baseBranchLength);
	Mesh treeMesh = meshFromGeometry(treeGeometry);
	freeGeometry(treeGeometry);
	return treeMesh;
}