#pragma once

#include "glutils.h"
#include "shadow.h"
#include "cameraController.h"
#include "character.h"

extern GLuint reflectionFrameBuffer;

void initSnow();

mat4 updateSnow(vec3 *reflectionDirection, const mat4 *projection, const mat4 *characterModel, const vec3* characterPosition);
void renderSnow(const mat4 *projection, const mat4 *view, const mat4 *reflectionView);

void cleanupSnow();