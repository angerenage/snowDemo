#include "character.h"

#include <stdlib.h>

typedef struct s_crystal {
	float radius;
	float topHeight;
	float bottomHeight;
	float gap;
	float innerScale;
	int segments;
	Plane plane;
} Crystal;

typedef struct s_triangle {
	vec3 A;
	vec3 B;
	vec3 C;
} Triangle;

typedef struct s_face {
	Triangle triangle;
	uint8_t material;
	uint8_t boneId;
} Face;

typedef enum e_axis {
	X = 0b001,
	Y = 0b010,
	Z = 0b100,
	MINUS = 0b1000,
} Axis;

typedef struct s_boneDefinition {
	vec3 position;
	Axis axis;
	vec3 localPosition, localScale;
	float length, radius;
	uint8_t parentId;
} BoneDefinition;

static const BoneDefinition characterDefinition[] = {
	{{0.0f, 0.7553f, 0.0f}, Y, {0.0f, 0.287f, 0.0f}, {1.0f, 1.0f, 0.713f}, 0.287f, 0.23f, 0},			// 1 : Lower body
	{{0.0f, 1.0662f, 0.0f}, Y | MINUS, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.713f}, 0.287f, 0.23f, 1},		// 2 : Upper body

	{{-0.1548f, 0.8317f, 0.0f}, Y, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 0.395f, 0.12f, 1},			// 3 : Upper left leg
	{{-0.1548f, 0.4164f, 0.0f}, Y, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 0.395f, 0.12f, 3},			// 4 : Lower left leg
	{{-0.1548f, 0.0214f, 0.0f}, Z, {0.0f, -0.0214f, 0.1958f}, {1.0f, 0.5f, 1.0f}, 0.221f, 0.10f, 4},	// 5 : Left foot

	{{0.1548f, 0.8317f, 0.0f}, Y, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 0.395f, 0.12f, 1},			// 6 : Upper right leg
	{{0.1548f, 0.4164f, 0.0f}, Y, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 0.395f, 0.12f, 6},			// 7 : Lower right leg
	{{0.1548f, 0.0214f, 0.0f}, Z, {0.0f, -0.0214f, 0.1958f}, {1.0f, 0.5f, 1.0f}, 0.221f, 0.10f, 7},		// 8 : Right foot

	{{-0.0910f, 1.3194f, 0.0f}, X | MINUS, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 0.395f, 0.10f, 2},	// 9 : Upper left arm
	{{-0.5063f, 1.3194f, 0.0f}, X | MINUS, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 0.395f, 0.10f, 9},	// 10 : Lower left arm
	
	{{0.0910f, 1.3194f, 0.0f}, X, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 0.395f, 0.10f, 2},			// 11 : Upper right arm
	{{0.5063f, 1.3194f, 0.0f}, X, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 0.395f, 0.10f, 11},			// 12 : Lower right arm

	{{0.0f, 1.3952f, 0.0f}, Y | MINUS, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 0.335f, 0.29f, 2},		// 13 : Head
};

static GLuint characterVAO;
static unsigned int characterVertexCount;
static Bone bones[sizeof(characterDefinition) / sizeof(BoneDefinition)] = {0};
Bone character = {0};

static float randomFloat(float min, float max) {
	return min + (max - min) * ((float)rand() / RAND_MAX);
}

static vec3 randomVector() {
	float x = randomFloat(0.0f, 1.0f);
	float y = randomFloat(0.0f, 1.0f) * 2.0f;
	float z = randomFloat(0.0f, 1.0f);
	return vec3_normalize((vec3){x, y, z});
}

static vec3 axisToRotation(Axis axis) {
	bool invert = axis & MINUS;
	float halfPi = (invert ? -1.0f : 1.0f) * (M_PI / 2.0f);
	switch (axis & ~MINUS) {
		case X:
			return (vec3){0.0, 0.0f, halfPi};
		case Y:
			return (vec3){invert ? M_PI : 0.0f, 0.0f, 0.0f};
		case Z:
			return (vec3){halfPi, 0.0f, 0.0f};
	}
}

static int reconstructTriangle(Triangle triangle, Plane plane, Triangle *triangles, bool invert) {
	vec3 intersections[3] = {0};
	int intersectionType = 0;
	if (intersectEdgeWithPlane(triangle.A, triangle.B, plane, &intersections[0])) {
		intersectionType += 1;
	}
	if (intersectEdgeWithPlane(triangle.A, triangle.C, plane, &intersections[1])) {
		intersectionType += 2;
	}
	if (intersectEdgeWithPlane(triangle.B, triangle.C, plane, &intersections[2])) {
		intersectionType += 3;
	}

	switch (intersectionType) {
		default: // No intersections
			if (invert) return 0;
			else {
				triangles[0] = (Triangle){triangle.A, triangle.C, triangle.B};
				return 1;
			}
			break;

		case 3: // Intersection on edge AB and AC
			if (invert) {
				triangles[0] = (Triangle){triangle.B, intersections[0], intersections[1]};
				triangles[1] = (Triangle){triangle.B, intersections[1], triangle.C};
				return 2;
			}
			else {
				triangles[0] = (Triangle){triangle.A, intersections[1], intersections[0]};
				return 1;
			}
			break;

		case 4: // Intersection on edge AB and BC
			if (invert) {
				triangles[0] = (Triangle){triangle.B, intersections[0], intersections[2]};
				return 1;
			}
			else {
				triangles[0] = (Triangle){triangle.A, intersections[2], intersections[0]};
				triangles[1] = (Triangle){triangle.A, triangle.C, intersections[2]};
				return 2;
			}
			break;

		case 5: // Intersection on edge AC and BC
			if (invert) {
				triangles[0] = (Triangle){triangle.C, intersections[2], intersections[1]};
				return 1;
			}
			else {
				triangles[0] = (Triangle){triangle.A, intersections[1], intersections[2]};
				triangles[1] = (Triangle){triangle.A, intersections[2], triangle.B};
				return 2;
			}
			break;
	}
}

static int reconstructHalfCrystal(Crystal crystal, Plane plane, Triangle *triangles, bool invert) {
	vec3 topVertex = (vec3){0.0f, 0.0f, 0.0f};
	vec3 bottomVertex = (vec3){0.0f, -crystal.bottomHeight - crystal.topHeight, 0.0f};

	vec3 ring[crystal.segments];
	for (int i = 0; i < crystal.segments; i++) {
		float angle = 2.0f * M_PI * i / crystal.segments;
		ring[i].x = crystal.radius * cos(angle);
		ring[i].y = -crystal.topHeight;
		ring[i].z = crystal.radius * sin(angle);
	}

	int triangleCount = 0;

	// Intersect the crystal with the plane
	for (int i = 0; i < crystal.segments; i++) {
		Triangle newTriangles[4] = {0};
		int newTriangleCount = reconstructTriangle((Triangle){topVertex, ring[(i + 1) % crystal.segments], ring[i]}, plane, newTriangles, invert);
		newTriangleCount += reconstructTriangle((Triangle){bottomVertex, ring[i], ring[(i + 1) % crystal.segments]}, plane, &newTriangles[newTriangleCount], !invert);

		for (int j = 0; j < newTriangleCount; j++) {
			triangles[triangleCount++] = newTriangles[j];
		}
	}

	return triangleCount;
}

static int reconstructInnerCrystal(Crystal crystal, Plane plane1, Plane plane2, Triangle *triangles) {
	Triangle outerTriangles[crystal.segments * 8];
	int innerFaceCount = 0;
	
	for (int i = 0; i < reconstructHalfCrystal(crystal, plane2, outerTriangles, false); i++) {
		Triangle newTriangles[2];
		int faceCount = reconstructTriangle(outerTriangles[i], plane1, newTriangles, true);

		float epsilon = -crystal.topHeight - 0.00001f;
		for (int j = 0; j < faceCount; j++) {
			if (newTriangles[j].A.y >= epsilon && newTriangles[j].B.y >= epsilon && newTriangles[j].B.y >= epsilon)
				triangles[innerFaceCount++] = (Triangle){newTriangles[j].A, newTriangles[j].C, newTriangles[j].B};
		}
	}

	for (int i = 0; i < reconstructHalfCrystal(crystal, plane1, outerTriangles, true); i++) {
		Triangle newTriangles[2];
		int faceCount = reconstructTriangle(outerTriangles[i], plane2, newTriangles, true);

		float epsilon = -crystal.topHeight + 0.00001f;
		for (int j = 0; j < faceCount; j++) {
			if (newTriangles[j].A.y <= epsilon && newTriangles[j].B.y <= epsilon && newTriangles[j].B.y <= epsilon)
				triangles[innerFaceCount++] = (Triangle){newTriangles[j].A, newTriangles[j].C, newTriangles[j].B};
		}
	}

	return innerFaceCount;
}

static int foundPointsOnPlane(int *pointsId, Triangle triangle, Plane plane) {
	int pointCount = 0;
	if (pointOnPlane(triangle.A, plane)) pointsId[pointCount++] = 0;
	if (pointOnPlane(triangle.B, plane)) pointsId[pointCount++] = 1;
	if (pointOnPlane(triangle.C, plane)) pointsId[pointCount++] = 2;
	return pointCount;
}

Face *generateCrystal(Crystal crystal, int *faceCount, int boneId) {
	Plane plane1 = {vec3_add(crystal.plane.point, vec3_scale(crystal.plane.normal, crystal.gap / 2.0f)), crystal.plane.normal};
	Plane plane2 = {vec3_add(crystal.plane.point, vec3_scale(crystal.plane.normal,  -crystal.gap / 2.0f)), crystal.plane.normal};

	Triangle triangles[crystal.segments * 12];

	// Construct the outer crystal
	int triangleCount = reconstructHalfCrystal(crystal, plane1, triangles, false);
	triangleCount += reconstructHalfCrystal(crystal, plane2, &triangles[triangleCount], true);

	// Construct the inner crystal
	Triangle innerTriangles[crystal.segments * 10];
	int innerTriangleCount = reconstructInnerCrystal(crystal, plane1, plane2, innerTriangles);

	for (int i = 0; i < innerTriangleCount; i++) {
		int pointsOnPlaneId[3] = {0};
		int pointCount = foundPointsOnPlane(pointsOnPlaneId, innerTriangles[i], plane1);
		if (pointCount != 2) pointCount = foundPointsOnPlane(pointsOnPlaneId, innerTriangles[i], plane2);

		Triangle currentTriangle = innerTriangles[i];

		// Scale down the inner triangle
		innerTriangles[i].A = vec3_scale(vec3_subtract(innerTriangles[i].A, crystal.plane.point), crystal.innerScale);
		innerTriangles[i].B = vec3_scale(vec3_subtract(innerTriangles[i].B, crystal.plane.point), crystal.innerScale);
		innerTriangles[i].C = vec3_scale(vec3_subtract(innerTriangles[i].C, crystal.plane.point), crystal.innerScale);

		// Scale up the inner triangles following the plane normal
		innerTriangles[i].A = vec3_add(crystal.plane.point, vec3_scaleAlongVector(innerTriangles[i].A, crystal.plane.normal, 1.0f / crystal.innerScale));
		innerTriangles[i].B = vec3_add(crystal.plane.point, vec3_scaleAlongVector(innerTriangles[i].B, crystal.plane.normal, 1.0f / crystal.innerScale));
		innerTriangles[i].C = vec3_add(crystal.plane.point, vec3_scaleAlongVector(innerTriangles[i].C, crystal.plane.normal, 1.0f / crystal.innerScale));

		if (pointCount == 2) {
			vec3 *oldPoints = (vec3*)&currentTriangle;
			vec3 *newPoints = (vec3*)&innerTriangles[i];
			triangles[triangleCount++] = (Triangle){oldPoints[pointsOnPlaneId[0]], oldPoints[pointsOnPlaneId[1]], newPoints[pointsOnPlaneId[0]]};
			triangles[triangleCount++] = (Triangle){oldPoints[pointsOnPlaneId[1]], newPoints[pointsOnPlaneId[1]], newPoints[pointsOnPlaneId[0]]};
		}
	}

	// Construct the faces
	*faceCount = triangleCount + innerTriangleCount;
	Face *faces = (Face*)malloc(sizeof(Face) * (*faceCount));

	for (int i = 0; i < triangleCount; i++) {
		faces[i] = (Face){triangles[i], 0, boneId};
	}

	for (int i = 0; i < innerTriangleCount; i++) {
		faces[triangleCount + i] = (Face){innerTriangles[i], 1, boneId};
	}

	return faces;
}

GLuint createCharacterVAO(const Face *faces, unsigned int faceCount) {
	typedef struct s_vertex {
		vec3 position;
		vec3 normal;
		uint8_t material;
		uint8_t boneId;
	} Vertex;

	characterVertexCount = faceCount * 3;
	Vertex *vertices = (Vertex*)malloc(sizeof(Vertex) * characterVertexCount);
	for (int i = 0; i < faceCount; i++) {
		vec3 normal = vec3_normalize(vec3_crossProduct(vec3_subtract(faces[i].triangle.B, faces[i].triangle.A), vec3_subtract(faces[i].triangle.C, faces[i].triangle.A)));
		vertices[i * 3 + 0] = (Vertex){faces[i].triangle.A, normal, faces[i].material, faces[i].boneId};
		vertices[i * 3 + 1] = (Vertex){faces[i].triangle.B, normal, faces[i].material, faces[i].boneId};
		vertices[i * 3 + 2] = (Vertex){faces[i].triangle.C, normal, faces[i].material, faces[i].boneId};
	}

	GLuint vao, vbo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * characterVertexCount, vertices, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, material));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, boneId));

	glBindVertexArray(0);

	return vao;
}

void initCharacter() {
	const float gap = 0.02f;

	Face *faces = NULL;
	int faceCount = 0;

	for (int i = 0; i < sizeof(characterDefinition) / sizeof(BoneDefinition); i++) {
		// Create the mesh
		const float topHeight = characterDefinition[i].length * randomFloat(0.3f, 0.7f);
		Crystal crystal = {
			characterDefinition[i].radius / 2.0f,
			topHeight,
			characterDefinition[i].length - topHeight,
			gap,
			0.9f,
			6,
			(Plane){(vec3){0.0f, -topHeight, 0.0f}, randomVector()}
		};

		mat4 transformation = scaleMatrix(characterDefinition[i].localScale);
		transformation = mat4_multiply(transformation, rotationMatrix(axisToRotation(characterDefinition[i].axis)));

		int newFaceCount = 0;
		Face *newFaces = generateCrystal(crystal, &newFaceCount, i);

		faces = (Face*)realloc(faces, sizeof(Face) * (faceCount + newFaceCount));
		for (int j = 0; j < newFaceCount; j++) {
			newFaces[j].triangle.A = vec3_add(characterDefinition[i].localPosition, transform(transformation, newFaces[j].triangle.A));
			newFaces[j].triangle.B = vec3_add(characterDefinition[i].localPosition, transform(transformation, newFaces[j].triangle.B));
			newFaces[j].triangle.C = vec3_add(characterDefinition[i].localPosition, transform(transformation, newFaces[j].triangle.C));
			
			faces[faceCount + j] = newFaces[j];
		}
		faceCount += newFaceCount;

		free(newFaces);

		// Create the bones
		bones[i] = (Bone){
			characterDefinition[i].position,
			(vec3){0.0f, 0.0f, 0.0f},
			0,
			NULL
		};

		int parentId = characterDefinition[i].parentId - 1;
		if (parentId >= 0) {
			bones[i].children = (Bone**)realloc(bones[i].children, sizeof(Bone*) * (bones[i].childrenCount + 1));
			bones[i].children[bones[i].childrenCount++] = &bones[i];
		}
	}

	characterVAO = createCharacterVAO(faces, faceCount);
	free(faces);
}

void renderCharacter(mat4 projection, mat4 view, mat4 model) {
	glUseProgram(characterShader);

	glUniformMatrix4fv(glGetUniformLocation(characterShader, "projection"), 1, GL_FALSE, &projection);
	glUniformMatrix4fv(glGetUniformLocation(characterShader, "view"), 1, GL_FALSE, &view);
	glUniformMatrix4fv(glGetUniformLocation(characterShader, "model"), 1, GL_FALSE, &model);

	int boneCount = sizeof(characterDefinition) / sizeof(BoneDefinition);
	mat4 bonesMatrix[boneCount];
	for (int i = 0; i < boneCount; i++) {
		mat4 position = translationMatrix(bones[i].position);
		mat4 rotation = rotationMatrix(bones[i].rotation);
		bonesMatrix[i] = mat4_multiply(position, rotation);
	}
	glUniformMatrix4fv(glGetUniformLocation(characterShader, "bones"), boneCount, GL_FALSE, bonesMatrix);

	glBindVertexArray(characterVAO);
	glDrawArrays(GL_TRIANGLES, 0, characterVertexCount);
	glBindVertexArray(0);

	glUseProgram(0);
}

void cleanupCharacter() {
	glDeleteVertexArrays(1, &characterVAO);
}