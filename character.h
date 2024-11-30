#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "geometry.h"
#include "glutils.h"
#include "ressource.h"

typedef struct s_bone {
	vec3 position;
	mat3 rotation;
	uint8_t parentID;
} Bone;

void initCharacter();
void loadAnimation(const char* path);
void updateAnimation(float time);
void renderCharacter(mat4 projection, mat4 view, vec3 pos, vec3 rot);
void cleanupCharacter();