#pragma once

#include "glutils.h"
#include "shadow.h"
#include "cameraController.h"
#include "character.h"

#define CHUNK_SIZE 10.0f
#define CHUNK_NBR_Z 10

extern GLuint reflectionFrameBuffer;

void initSnow();

mat4 updateSnow(vec3* restrict reflectionDirection, const mat4* restrict const projection, const mat4* restrict const characterModel);
void renderSnow(const mat4* restrict const projection, const mat4* restrict const view, int chunkZ);
void renderIce(const mat4* restrict const projection);

void cleanupSnow();