#pragma once

#include "glutils.h"

#define CHUNK_SIZE 10.0f
#define CHUNK_NBR_Z 10

extern GLuint reflectionFrameBuffer;

void initSnow();

mat4 updateSnow(vec3* restrict reflectionDirection, const mat4* restrict const characterModel);
void renderSnow(const mat4* restrict const view, int chunkZ);
void renderIce();

void cleanupSnow();