#pragma once

#include <stdbool.h>

#include "geometry.h"
#include "cameraController.h"

#define FULLSCREEN

extern bool running;
extern int currentSceneId;

double getTime();

void initWindow(vec2 size);
void handleEvents();
void swapBuffers();
void cleanupWindow();