#pragma once

#include "glutils.h"

Mesh generateGrid(vec2 size, int subdivision);
GLuint generateTerrainHeight(const vec2 *pos);