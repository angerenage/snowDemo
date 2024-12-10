#pragma once

#include "glutils.h"
#include "shadow.h"
#include "cameraController.h"
#include "character.h"

void initSnow();
void updateSnow(const mat4 *characterModel, vec3 characterPosition);
void renderSnow(GLuint shader, const mat4 *projection, const mat4 *view);
void cleanupSnow();