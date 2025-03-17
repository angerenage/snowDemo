#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "glutils.h"
#include "geometry.h"

Mesh generateTree(float height, float scaleFactor, int segments, int numBranches, float baseBranchLength);