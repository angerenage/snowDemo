#include "character.h"

#include <stdlib.h>

#include "snow.h"

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
	X = 0,
	Y = 1,
	Z = 2,
	MINUS = 4,
	DOWN = 8,
	INVERT = 16
} Axis;

struct s_GpuBone {
	vec4 position;         // vec3 + padding
	vec4 lightPosition;    // vec3 + padding
	vec4 rotation[3];      // mat3 -> 3 vec3 aligned to 16 bytes
	uint32_t parentID;     // uint8_t + padding
	uint8_t _padding[12];  // padding to 16 bytes
};

typedef struct s_boneDefinition {
	vec3 position;
	Axis axis;
	vec3 localPosition, localScale;
	float length, radius;
	uint8_t parentId;
} BoneDefinition;

static const BoneDefinition characterDefinition[] = {
	{{0.0f, 0.0f, 0.0f}, Y, {0.0f, 0.287f, 0.0f}, {0.713f, 1.0f, 1.0f}, 0.287f, 0.23f, 0},						// 0 : Lower body
	{{0.0f, 0.3109f, 0.0f}, Y | MINUS | INVERT, {0.0f, 0.0f, 0.0f}, {0.713f, 1.0f, 1.0f}, 0.287f, 0.23f, 0},	// 1 : Upper body

	{{0.0f, 0.0764f, 0.1248f}, Y | MINUS | DOWN, {0.0f, -0.395f, 0.0f}, {1.0f, 1.0f, 1.0f}, 0.395f, 0.12f, 0},	// 2 : Upper left leg
	{{0.0f, -0.4153f, 0.0f}, Y | DOWN | INVERT, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 0.4164f, 0.12f, 2},		// 3 : Lower left leg

	{{0.0f, 0.0764f, -0.1248f}, Y | MINUS | DOWN, {0.0f, -0.395f, 0.0f}, {1.0f, 1.0f, 1.0f}, 0.395f, 0.12f, 0},	// 4 : Upper right leg
	{{0.0f, -0.4153f, 0.0f}, Y | DOWN | INVERT, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 0.4164f, 0.12f, 4},		// 5 : Lower right leg

	{{0.0f, 0.2533f, 0.1203f}, Z, {0.0f, 0.0f, 0.395f}, {1.0f, 1.0f, 1.0f}, 0.395f, 0.10f, 1},					// 6 : Upper left arm
	{{0.0f, 0.0f, 0.4153f}, Z | MINUS | INVERT, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 0.395f, 0.10f, 6},		// 7 : Lower left arm

	{{0.0f, 0.2533f, -0.1203f}, Z | MINUS | DOWN, {0.0f, 0.0f, -0.395f}, {1.0f, 1.0f, 1.0f}, 0.395f, 0.10f, 1},	// 8 : Upper right arm
	{{0.0f, 0.0f, -0.4153f}, Z | DOWN | INVERT, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 0.395f, 0.10f, 8},		// 9 : Lower right arm

	{{0.0f, 0.3290f, 0.0f}, Y | MINUS | INVERT, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 0.335f, 0.29f, 1},		// 10 : Head
};

static GLuint characterVAO;
static unsigned int characterVertexCount;

const unsigned int boneNumber = sizeof(characterDefinition) / sizeof(BoneDefinition);
static Bone bones[sizeof(characterDefinition) / sizeof(BoneDefinition)] = {0};
static GLuint boneSSBO;

typedef struct s_frame {
	vec3 position;
	Quaternion rotations[sizeof(characterDefinition) / sizeof(BoneDefinition)];
} Frame;

static Frame* animation = NULL;
static unsigned int animationLength = 0;

vec3 characterPosition = {0.0f, 0.0f, 0.0f};
float currentZOffset = 0.0f;

static vec3 randomVector() {
	float x = randomFloat(0.0f, 1.0f);
	float y = randomFloat(0.0f, 1.0f) * 2.0f;
	float z = randomFloat(0.0f, 1.0f);
	return vec3_normalize((vec3){x, y, z});
}

static vec3 axisToRotation(Axis axis) {
	bool invert = axis & MINUS;
	float halfPi = (invert ? -1.0f : 1.0f) * ((float)M_PI / 2.0f);
	switch (axis & ~MINUS & ~DOWN & ~INVERT) {
		case X:
			return (vec3){0.0, 0.0f, halfPi};
		case Y:
			return (vec3){invert ? (float)M_PI : 0.0f, 0.0f, 0.0f};
		case Z:
			return (vec3){halfPi, 0.0f, 0.0f};
	}
	return (vec3){0.0f, 0.0f, 0.0f};
}

static vec3 axisToVector(Axis axis, float length) {
	float result[3] = {0.0f, 0.0f, 0.0f};
	result[axis & ~MINUS & ~DOWN & ~INVERT] = axis & DOWN ? -length : length;
	return *(vec3*)result;
}

static int reconstructTriangle(Triangle triangle, Plane plane, Triangle* restrict triangles, bool invert) {
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
				triangles[0] = (Triangle){triangle.A, triangle.B, triangle.C};
				return 1;
			}
			break;

		case 3: // Intersection on edge AB and AC
			if (invert) {
				triangles[0] = (Triangle){triangle.B, intersections[1], intersections[0]};
				triangles[1] = (Triangle){triangle.B, triangle.C, intersections[1]};
				return 2;
			}
			else {
				triangles[0] = (Triangle){triangle.A, intersections[0], intersections[1]};
				return 1;
			}
			break;

		case 4: // Intersection on edge AB and BC
			if (invert) {
				triangles[0] = (Triangle){triangle.B, intersections[2], intersections[0]};
				return 1;
			}
			else {
				triangles[0] = (Triangle){triangle.A, intersections[0], intersections[2]};
				triangles[1] = (Triangle){triangle.A, intersections[2], triangle.C};
				return 2;
			}
			break;

		case 5: // Intersection on edge AC and BC
			if (invert) {
				triangles[0] = (Triangle){triangle.C, intersections[1], intersections[2]};
				return 1;
			}
			else {
				triangles[0] = (Triangle){triangle.A, intersections[2], intersections[1]};
				triangles[1] = (Triangle){triangle.A, triangle.B, intersections[2]};
				return 2;
			}
			break;
	}
}

static int reconstructHalfCrystal(Crystal crystal, Plane plane, Triangle* restrict triangles, bool invert) {
	vec3 topVertex = (vec3){0.0f, 0.0f, 0.0f};
	vec3 bottomVertex = (vec3){0.0f, -crystal.bottomHeight - crystal.topHeight, 0.0f};

	vec3 *ring = malloc(sizeof(vec3) * crystal.segments);
	for (int i = 0; i < crystal.segments; i++) {
		float angle = 2.0f * (float)M_PI * i / crystal.segments;
		ring[i].x = crystal.radius * cosf(angle);
		ring[i].y = -crystal.topHeight;
		ring[i].z = crystal.radius * sinf(angle);
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

	free(ring);

	return triangleCount;
}

static int reconstructInnerCrystal(Crystal crystal, Plane plane1, Plane plane2, Triangle* restrict triangles) {
	Triangle *outerTriangles = (Triangle*)malloc(sizeof(Triangle) * crystal.segments * 8);
	int innerFaceCount = 0;
	
	for (int i = 0; i < reconstructHalfCrystal(crystal, plane2, outerTriangles, false); i++) {
		Triangle newTriangles[2];
		int faceCount = reconstructTriangle(outerTriangles[i], plane1, newTriangles, true);

		float epsilon = -crystal.topHeight - 0.00001f;
		for (int j = 0; j < faceCount; j++) {
			if (newTriangles[j].A.y >= epsilon && newTriangles[j].B.y >= epsilon && newTriangles[j].B.y >= epsilon)
				triangles[innerFaceCount++] = newTriangles[j];
		}
	}

	for (int i = 0; i < reconstructHalfCrystal(crystal, plane1, outerTriangles, true); i++) {
		Triangle newTriangles[2];
		int faceCount = reconstructTriangle(outerTriangles[i], plane2, newTriangles, true);

		float epsilon = -crystal.topHeight + 0.00001f;
		for (int j = 0; j < faceCount; j++) {
			if (newTriangles[j].A.y <= epsilon && newTriangles[j].B.y <= epsilon && newTriangles[j].B.y <= epsilon)
				triangles[innerFaceCount++] = newTriangles[j];
		}
	}

	free(outerTriangles);

	return innerFaceCount;
}

static int foundPointsOnPlane(int* restrict pointsId, Triangle triangle, Plane plane) {
	int pointCount = 0;
	if (pointOnPlane(triangle.A, plane)) pointsId[pointCount++] = 0;
	if (pointOnPlane(triangle.B, plane)) pointsId[pointCount++] = 1;
	if (pointOnPlane(triangle.C, plane)) pointsId[pointCount++] = 2;
	return pointCount;
}

static Face* generateCrystal(Crystal crystal, int* restrict faceCount, uint8_t boneId) {
	Plane plane1 = {vec3_add(crystal.plane.point, vec3_scale(crystal.plane.normal, crystal.gap / 2.0f)), crystal.plane.normal};
	Plane plane2 = {vec3_add(crystal.plane.point, vec3_scale(crystal.plane.normal,  -crystal.gap / 2.0f)), crystal.plane.normal};

	Triangle *triangles = (Triangle*)malloc(sizeof(Triangle) * crystal.segments * 12);

	// Construct the outer crystal
	int triangleCount = reconstructHalfCrystal(crystal, plane1, triangles, false);
	triangleCount += reconstructHalfCrystal(crystal, plane2, &triangles[triangleCount], true);

	// Construct the inner crystal
	Triangle *innerTriangles = (Triangle*)malloc(sizeof(Triangle) * crystal.segments * 10);
	int innerTriangleCount = reconstructInnerCrystal(crystal, plane1, plane2, innerTriangles);

	for (int i = 0; i < innerTriangleCount; i++) {
		int pointsOnPlaneId[3] = {0};
		int pointCount = foundPointsOnPlane(pointsOnPlaneId, innerTriangles[i], plane1);
		if (pointCount != 2) pointCount = foundPointsOnPlane(pointsOnPlaneId, innerTriangles[i], plane2);

		Triangle currentTriangle = innerTriangles[i];

		// Scale down the inner triangle
		innerTriangles[i].A = vec3_scale(vec3_sub(innerTriangles[i].A, crystal.plane.point), crystal.innerScale);
		innerTriangles[i].B = vec3_scale(vec3_sub(innerTriangles[i].B, crystal.plane.point), crystal.innerScale);
		innerTriangles[i].C = vec3_scale(vec3_sub(innerTriangles[i].C, crystal.plane.point), crystal.innerScale);

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

	free(triangles);
	free(innerTriangles);

	return faces;
}

static GLuint createCharacterVAO(const Face* restrict const faces, unsigned int faceCount) {
	typedef struct s_vertex {
		vec3 position;
		vec3 normal;
		uint8_t material;
		uint8_t boneId;
	} Vertex;

	characterVertexCount = faceCount * 3;
	Vertex *vertices = (Vertex*)malloc(sizeof(Vertex) * characterVertexCount);
	for (unsigned int i = 0; i < faceCount; i++) {
		vec3 normal = vec3_normalize(vec3_cross(vec3_sub(faces[i].triangle.C, faces[i].triangle.A), vec3_sub(faces[i].triangle.B, faces[i].triangle.A)));
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
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)offsetof(Vertex, material));
	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 1, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)offsetof(Vertex, boneId));

	glBindVertexArray(0);

	return vao;
}

void initCharacter() {
	const float gap = 0.02f;

	Face *faces = NULL;
	int faceCount = 0;

	for (unsigned int i = 0; i < boneNumber; i++) {
		// Create the mesh
		const float topHeight = characterDefinition[i].length * randomFloat(0.1f, 0.4f);
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
		mat4 rotation = rotationMatrix(axisToRotation(characterDefinition[i].axis));
		transformation = mat4_multiply(&transformation, &rotation);

		int newFaceCount = 0;
		Face *newFaces = generateCrystal(crystal, &newFaceCount, (uint8_t)i);


#ifdef EXPORT_TO_BLENDER
		vec3 position = {0};
		int currentBoneId = i;
		while (characterDefinition[currentBoneId].parentId != currentBoneId) {
			position = vec3_add(position, characterDefinition[currentBoneId].position);
			currentBoneId = characterDefinition[currentBoneId].parentId;
		}

		printf("b bone_%d %f %f %f", i, position.x, position.y, position.z);
		if (characterDefinition[i].parentId != i) printf(" bone_%d\n", characterDefinition[i].parentId);
		else printf("\n");
#endif


		faces = (Face*)realloc(faces, sizeof(Face) * (faceCount + newFaceCount));
		for (int j = 0; j < newFaceCount; j++) {
			vec3 localPosition = characterDefinition[i].localPosition;
			newFaces[j].triangle.A = vec3_add(localPosition, transform(transformation, newFaces[j].triangle.A));
			newFaces[j].triangle.B = vec3_add(localPosition, transform(transformation, newFaces[j].triangle.B));
			newFaces[j].triangle.C = vec3_add(localPosition, transform(transformation, newFaces[j].triangle.C));
			
			faces[faceCount + j] = newFaces[j];


#ifdef EXPORT_TO_BLENDER
			vec3 A = vec3_add(position, newFaces[j].triangle.A);
			vec3 B = vec3_add(position, newFaces[j].triangle.B);
			vec3 C = vec3_add(position, newFaces[j].triangle.C);
			printf("v %f %f %f\n", A.x, A.y, A.z);
			printf("v %f %f %f\n", B.x, B.y, B.z);
			printf("v %f %f %f\n", C.x, C.y, C.z);
			printf("f %d %d %d m %d\n", (faceCount + j) * 3, (faceCount + j) * 3 + 1, (faceCount + j) * 3 + 2, newFaces[j].material);
#endif
		}
		faceCount += newFaceCount;

		free(newFaces);

		// Create the bones
		bones[i] = (Bone){
			characterDefinition[i].position,
			axisToVector(characterDefinition[i].axis, characterDefinition[i].axis & INVERT ? crystal.topHeight : crystal.bottomHeight),
			mat3_identity(),
			characterDefinition[i].parentId
		};
	}

	characterVAO = createCharacterVAO(faces, faceCount);
	free(faces);

	boneSSBO = createSSBO(sizeof(struct s_GpuBone) * boneNumber, 1);
}

void loadAnimation(const Ressource* restrict const anim) {
	animation = (Frame*)anim->data;
	animationLength = (unsigned int)(anim->size / sizeof(Frame));
}

void updateCharacter(float time) {
	characterPosition.z = time * 3.5f;

	const float patchSize = (float)(CHUNK_NBR_Z * CHUNK_SIZE);
	if (characterPosition.z - currentZOffset >= patchSize) currentZOffset = floorf(characterPosition.z / patchSize) * patchSize;

	if (!animation) return;
	
	int currentFrameId = (int)(time * 30.0f) % animationLength;
	int nextFrameId = (currentFrameId + 1) % animationLength;

	Frame currentFrame = animation[currentFrameId];
	Frame nextFrame = animation[nextFrameId];

	bones[0].position = vec3_lerp(currentFrame.position, nextFrame.position, time - (int)time);
	for (unsigned int i = 0; i < boneNumber; i++) {
		Quaternion rot = quat_lerp(currentFrame.rotations[i], nextFrame.rotations[i], time - (int)time);
		bones[i].rotation = mat3_quaternion(rot);
	}

	struct s_GpuBone *gpuBones = malloc(sizeof(struct s_GpuBone) * boneNumber);
	for (unsigned int i = 0; i < boneNumber; i++) {
		struct s_GpuBone *dst = &gpuBones[i];
		Bone *src = &bones[i];

		dst->position.x = src->position.x;
		dst->position.y = src->position.y;
		dst->position.z = src->position.z;
		dst->position.w = 0.0f;

		dst->lightPosition.x = src->lightPosition.x;
		dst->lightPosition.y = src->lightPosition.y;
		dst->lightPosition.z = src->lightPosition.z;
		dst->lightPosition.w = 0.0f;

		for (int col = 0; col < 3; col++) {
			dst->rotation[col].x = src->rotation.m[col][0];
			dst->rotation[col].y = src->rotation.m[col][1];
			dst->rotation[col].z = src->rotation.m[col][2];
			dst->rotation[col].w = 0.0f;
		}

		dst->parentID = (uint32_t)(src->parentID);
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, boneSSBO);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(struct s_GpuBone) * boneNumber, gpuBones);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	free(gpuBones);
}

void renderCharacter(GLuint shader, const mat4* restrict const projection, const mat4* restrict const view, const mat4* restrict const model) {
	glUseProgram(shader);

	glUniformMatrix4fv(glGetUniformLocation(shader, uniform_projection), 1, GL_FALSE, (GLfloat*)projection);
	glUniformMatrix4fv(glGetUniformLocation(shader, uniform_view), 1, GL_FALSE, (GLfloat*)view);
	glUniformMatrix4fv(glGetUniformLocation(shader, uniform_model), 1, GL_FALSE, (GLfloat*)model);
	glUniform3fv(glGetUniformLocation(shader, uniform_lightPos), 1, (GLfloat*)&lightPosition);

	glBindVertexArray(characterVAO);
	glDrawArrays(GL_TRIANGLES, 0, characterVertexCount);
	glBindVertexArray(0);

	glUseProgram(0);
}

void cleanupCharacter() {
	glDeleteVertexArrays(1, &characterVAO);
	glDeleteBuffers(1, &boneSSBO);
}