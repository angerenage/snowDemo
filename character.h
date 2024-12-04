#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "geometry.h"
#include "glutils.h"
#include "ressource.h"
#include "shadow.h"

typedef struct s_bone {
	vec3 position;
	vec3 lightPosition;
	mat3 rotation;
	uint8_t parentID;
} Bone;

void initCharacter();
void loadAnimation(const char* path);
void updateAnimation(float time);
void renderCharacter(GLuint shader, mat4 projection, mat4 view, mat4 model);
void cleanupCharacter();