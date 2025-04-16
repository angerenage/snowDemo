#pragma once

#include <stdbool.h>
#include "geometry.h"
#include "cameraController.h"

extern bool running;
extern int currentSceneId;

extern mat4 projection;

float getTime();

void initWindow(vec2 size);
void handleEvents();
void swapBuffers();
void cleanupWindow();