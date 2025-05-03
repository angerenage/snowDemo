#pragma once

#include "glutils.h"
#include "geometry.h"

#define SHADOW_MAP_SIZE 8192

extern mat4 shadowProjection;
extern mat4 shadowView;
extern vec3 lightPosition;

extern GLuint shadowMap;
extern GLuint shadowFBO;

void initShadow();
void clearShadow();

void updateLight(float time, bool isDay);

void renderLights(const mat4* restrict const view, float time);

void cleanupShadow();