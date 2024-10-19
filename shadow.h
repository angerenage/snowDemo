#pragma once

#include "glutils.h"

#define SHADOW_MAP_SIZE 8192

extern mat4 shadowProjection;
extern mat4 shadowView;
extern vec3 sunPosition;

extern GLuint shadowMap;
extern GLuint shadowFBO;

void initShadow();
void clearShadow();

void updateLight(float time);

void cleanupShadow();