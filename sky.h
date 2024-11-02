#pragma once

#include "glutils.h"
#include "cameraController.h"

void initSky();

void updateSky(const vec3* sunPosition, const vec2* screenSize, float ftime);
void renderSky(const mat4* projection);

void cleanupSky();