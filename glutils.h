#pragma once

#include <glad/glad.h>
#include <stddef.h>

#include "geometry.h"
#include "shader.h"

typedef struct s_mesh {
	GLuint VAO;
	int vertexCount;
	int indexCount;
} Mesh;

typedef struct s_instancedMesh {
	GLuint VAO;
	GLuint instanceVBO;
	int vertexCount;
	int indexCount;
	int instanceCount;
} InstancedMesh;

extern GLuint rnoiseTexture;

void initUtils();

void renderScreenQuad();

GLuint createTexture(int width, int height);
GLuint createTextureR(int width, int height);
GLuint createTextureDepth(int width, int height);
GLuint createTextureStencil(int width, int height);
GLuint createTextureArray(int width, int height, int layer);
GLuint createTextureArrayRG(int width, int height, int layer);
GLuint createCubeMap(int width, int height);
GLuint createFramebuffer(GLuint texture);
GLuint createFramebufferDepth(GLuint depth);
GLuint createFramebufferMultisampleDepth(GLuint depth, GLuint color);
GLuint createFramebufferDepthStencil(GLuint depthStencil, GLuint color);

GLuint createIndexedVAO(const vec3* restrict const vertices, int vertexCount, const unsigned int* restrict const indices, unsigned int indexCount);
GLuint createVAO(const vec3* restrict const vertices, unsigned int vertexCount);
GLuint setupInstanceBuffer(GLuint instanceVAO, const vec3* restrict const positions, unsigned int positionsCount);

GLuint createSSBO(size_t size, GLuint index);

void checkOpenGLError();
void cleanupUtils();
void freeMesh(Mesh m);
void freeInstancedMesh(InstancedMesh m);