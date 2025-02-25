#pragma once

#include "glutils.h"
#include "cameraController.h"

void initSky();

void updateSky(const vec3* sunPosition, const vec2* screenSize, float ftime, const vec3* updateDirection);
void renderSky(const mat4* projection, const mat4* view);

void cleanupSky();