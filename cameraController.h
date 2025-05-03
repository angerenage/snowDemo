#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "geometry.h"
#include "character.h"

extern vec2 screenSize;

extern float lastMouseX;
extern float lastMouseY;
extern bool firstMouse;

extern mat4 cameraView;
extern vec3 cameraPos;
extern vec3 cameraDirection;

void moveCamera(float xoffset, float yoffset);
void defaultCameraTransforms(vec3* restrict pos, vec3* restrict dir, float distance, vec2 angles);
void updateCamera();

mat4 reflectionCameraMatrix(vec3* restrict reflectionDirection, float height);