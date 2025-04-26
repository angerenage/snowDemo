#include "shadow.h"

mat4 shadowProjection;
mat4 shadowView;
vec3 lightPosition;
static vec3 sunPosition;
static vec3 moonPosition = {0.6f, 1.0f, 0.8f};

GLuint shadowMap;
GLuint shadowFBO;

void initShadow() {
	shadowProjection = orthographicMatrix(-30.0f, 10.0f, -30.0f, 10.0f, -100.0f, 100.0f);
	
	shadowMap = createTextureDepth(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

#ifdef DEBUG
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "Error: Shadow framebuffer is not complete\n");
	}
#endif
}

void clearShadow() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void updateLight(float time, bool isDay) {
	const float sunTime = time / 20.0f;
	sunPosition = (vec3){sinf(sunTime) + cosf(sunTime), cosf(sunTime) - sinf(sunTime), 0.0};
	lightPosition = isDay ? sunPosition : moonPosition;

	shadowView = viewMatrix(vec3_add(characterPosition, lightPosition), characterPosition, (vec3){0.0, 1.0, 0.0});
}

void cleanupShadow() {
	glDeleteFramebuffers(1, &shadowFBO);
	glDeleteTextures(1, &shadowMap);
}