#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "geometry.h"

extern float cameraYaw;
extern float cameraPitch;
extern float lastMouseX;
extern float lastMouseY;
extern bool firstMouse;

mat4 getViewMatrix();
void defaultCameraTransforms(vec3 *pos, vec3 *dir, float distance, vec2 angles);