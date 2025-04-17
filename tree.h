#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "glutils.h"
#include "geometry.h"
#include "shadow.h"
#include "snow.h"

void initTrees();
void renderTrees(GLuint shader, const mat4* projection, const mat4* view, const vec3* lightPos, int chunkZ);
void cleanupTrees();