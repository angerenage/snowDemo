#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "geometry.h"
#include "glutils.h"

typedef struct s_bone {
	vec3 position;
	vec3 rotation;
	int childrenCount;
	struct s_bone **children;
} Bone;

extern Bone character;

void initCharacter();
void renderCharacter(mat4 projection, mat4 view, mat4 model);
void cleanupCharacter();