#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "glutils.h"
#include "geometry.h"
#include "shadow.h"
#include "snow.h"

void initTrees();
void renderTrees(GLuint shader, const mat4* restrict const projection, const mat4* restrict const view, const vec3* restrict const lightPos, int chunkZ);
void cleanupTrees();