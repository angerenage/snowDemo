#pragma once

#include "glutils.h"

Mesh generateGrid(vec2 size, int subdivision, float yOffset);
GLuint generateTerrainHeight(const vec2 *pos);