#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "geometry.h"
#include "glutils.h"
#include "ressource.h"
#include "shadow.h"

enum animation {
	ANIM_RUNNING,
};

typedef struct s_bone {
	vec3 position;
	vec3 lightPosition;
	mat3 rotation;
	uint8_t parentID;
} Bone;

void initCharacter();
void loadAnimation(const Ressource *anim);
void updateAnimation(float time);
void renderCharacter(GLuint shader, const mat4* projection, const mat4* view, const mat4* model);
void cleanupCharacter();