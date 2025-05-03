#pragma once

#include "glutils.h"
#include "geometry.h"

void initTrees();
void renderTrees(GLuint shader, const mat4* restrict const projection, const mat4* restrict const view, const vec3* restrict const lightPos, int chunkZ);
void cleanupTrees();