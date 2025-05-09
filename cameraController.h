#pragma once

#include "geometry.h"

extern vec2 screenSize;

extern float lastMouseX;
extern float lastMouseY;
extern bool firstMouse;

extern mat4 projection;
extern mat4 cameraView;
extern vec3 cameraPos;
extern vec3 cameraDirection;

void moveCamera(float xoffset, float yoffset);
void updateCamera();

mat4 reflectionCameraMatrix(vec3* restrict reflectionDirection, float height);