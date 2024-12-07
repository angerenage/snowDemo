#pragma once

#include "glutils.h"
#include "shadow.h"
#include "cameraController.h"

void initSnow();
void renderSnow(GLuint shader, const mat4 *projection, const mat4 *view);
void cleanupSnow();