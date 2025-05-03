#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "geometry.h"
#include "glutils.h"
#include "resources.h"
#include "shadow.h"
#include "shader.h"

enum animation {
	ANIM_RUNNING,
};

typedef struct s_bone {
	vec3 position;
	vec3 lightPosition;
	mat3 rotation;
	uint8_t parentID;
} Bone;

extern const unsigned int boneNumber;

extern vec3 characterPosition;
extern float currentZOffset;

void initCharacter();
void loadAnimation(const Ressource *anim);
void updateCharacter(float time);
void renderCharacter(GLuint shader, const mat4* projection, const mat4* view, const mat4* model);
void cleanupCharacter();