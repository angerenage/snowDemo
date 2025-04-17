#pragma once

#include "glutils.h"
#include "shadow.h"
#include "cameraController.h"
#include "character.h"

#define CHUNK_SIZE 10.0f
#define CHUNK_NBR_Z 10

extern GLuint reflectionFrameBuffer;

void initSnow();

mat4 updateSnow(vec3 *reflectionDirection, const mat4 *projection, const mat4 *characterModel);
void renderSnow(const mat4 *projection, const mat4 *view, const mat4 *reflectionView, int chunkZ);

void cleanupSnow();