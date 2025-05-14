#pragma once

#include <stdint.h>

#include "geometry.h"
#include "glutils.h"
#include <resources.h>

typedef struct s_bone {
	vec3 position;
	vec3 lightPosition;
	mat3 rotation;
	uint8_t parentID;
} Bone;

extern const unsigned int boneNumber;

extern vec3 characterPosition;
extern float currentZOffset;

extern unsigned int animationID;

void initCharacter();
void loadAnimation(const Ressource* restrict const anim);
void updateCharacter(float time);
void renderCharacter(GLuint shader, const mat4* restrict const projection, const mat4* restrict const view, const mat4* restrict const model);
void cleanupCharacter();