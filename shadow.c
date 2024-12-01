#include "shadow.h"

mat4 shadowProjection;
mat4 shadowView;
vec3 sunPosition;

GLuint shadowMap;
GLuint shadowFBO;

void initShadow() {
	shadowProjection = orthographicMatrix(-10.0f, 10.0f, -10.0f, 10.0f, -100.0f, 100.0f);
	
	shadowMap = createTextureDepth(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "Error: Shadow framebuffer is not complete\n");
	}
}

void clearShadow() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void updateLight(float time) {
	const float sunTime = time / 20.0f;
	sunPosition = (vec3){sin(sunTime) + cos(sunTime), cos(sunTime) - sin(sunTime), 0.0};
	shadowView = viewMatrix(sunPosition, (vec3){0.0, 0.0, 0.0}, (vec3){0.0, 1.0, 0.0});
}

void cleanupShadow() {
	glDeleteFramebuffers(1, &shadowFBO);
	glDeleteTextures(1, &shadowMap);
}