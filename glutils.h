#pragma once

#include <glad/glad.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <GL/gl.h>

#include <stdlib.h>
#include <stdio.h>

#include "geometry.h"
#include "shader.h"

typedef struct mesh_s {
	GLuint VAO;
	int vertexCount;
	int indexCount;
} Mesh;

extern Display *display;
extern Window window;
extern Atom wmDelete;

extern GLuint rnoiseTexture;

void initWindow(vec2 size);
void cleanupWindow();

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

GLuint createIndexedVAO(const vec3 *vertices, int vertexCount, const unsigned int *indices, unsigned int indexCount);
GLuint createVAO(const vec3 *vertices, unsigned int vertexCount);
GLuint setupInstanceBuffer(GLuint instanceVAO, const vec3 *positions, unsigned int positionsCount);

GLuint createSSBO(size_t size, GLuint index);

void checkOpenGLError();
void cleanupUtils();
void freeMesh(Mesh m);