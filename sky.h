#pragma once

#include "glutils.h"
#include "cameraController.h"
#include "shader.h"

void initSky();

void updateSky(float ftime, bool isDay, const vec3* updateDirection);
void renderSky(const mat4* projection, const mat4* view);

void cleanupSky();