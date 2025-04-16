#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "glutils.h"
#include "geometry.h"
#include "shadow.h"

Mesh generateTree(float height, float scaleFactor, int segments, int numBranches, float baseBranchLength);

mat4* generateTreeInstances(int rows, int cols, float spacing);

InstancedMesh bindTreeInstances(const Mesh* tree, const mat4* instances, int instanceCount);

void renderTrees(GLuint shader, const InstancedMesh* trees, const mat4* projection, const mat4* view, const mat4* model, const vec3* lightPos);