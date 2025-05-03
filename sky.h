#pragma once

#include "glutils.h"
#include "cameraController.h"
#include "shader.h"

void initSky();

void updateSky(float ftime, bool isDay, const vec3* restrict const updateDirection);
void renderSky(const mat4* restrict const projection, const mat4* restrict const view);

void cleanupSky();