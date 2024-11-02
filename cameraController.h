#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "geometry.h"

extern float lastMouseX;
extern float lastMouseY;
extern bool firstMouse;

extern mat4 view;
extern vec3 cameraPos;
extern vec3 cameraDirection;

void updateCamera(float xoffset, float yoffset);
void defaultCameraTransforms(vec3 *pos, vec3 *dir, float distance, vec2 angles);