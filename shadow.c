#include "shadow.h"

#include <stdio.h>

#include "character.h"
#include "cameraController.h"

#define LIGHTS_NUMBER 10

mat4 shadowProjection;
mat4 shadowView;
vec3 lightPosition;
static vec3 sunPosition;
static vec3 moonPosition = {0.6f, 1.0f, 0.8f};

GLuint shadowMap;
GLuint shadowFBO;

static GLuint pointLightSSBO;
static GLuint pointLightVAO;

typedef struct s_light {
	vec3 origin;
	vec3 direction;
	vec2 interval;
} Light;

static const Light lightParameters[LIGHTS_NUMBER] = {
	{ { -0.8f, 1.0f, -0.2f   }, {  0.8f, 5.0f, 14.0f }, { 5.0f,  1.5f } },
	{ { -0.2f, 1.0f,  12.4f  }, { -0.1f, 5.7f, 13.9f }, { 10.0f, 1.8f } },
	{ { -0.7f, 1.0f,  29.7f  }, {  0.6f, 5.0f, 14.1f }, { 15.0f, 1.8f } },
	{ { -1.9f, 1.0f,  51.6f  }, {  1.4f, 4.9f, 14.1f }, { 20.0f, 1.5f } },
	{ {  1.3f, 1.0f,  65.7f  }, { -0.9f, 4.3f, 14.7f }, { 25.0f, 1.5f } },
	{ {  1.2f, 1.0f,  84.5f  }, { -0.7f, 4.2f, 14.4f }, { 30.0f, 1.4f } },
	{ {  1.7f, 1.0f,  104.2f }, { -1.4f, 4.5f, 14.3f }, { 35.0f, 1.1f } }, 
	{ { -1.1f, 1.0f,  118.2f }, {  0.6f, 4.8f, 14.2f }, { 40.0f, 1.3f } },
	{ {  1.4f, 1.0f,  140.4f }, { -1.2f, 5.0f, 14.1f }, { 45.0f, 0.9f } },
	{ { -1.7f, 1.0f,  154.4f }, {  1.3f, 5.8f, 13.8f }, { 50.0f, 0.7f } },
};

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

	unsigned int lightNumber = LIGHTS_NUMBER + boneNumber;
	pointLightSSBO = createSSBO(sizeof(vec4) * 2 * lightNumber, 0);

	GLuint vbo;
	glGenVertexArrays(1, &pointLightVAO);
	glBindVertexArray(pointLightVAO);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Light) * LIGHTS_NUMBER, lightParameters, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Light), (void*)offsetof(Light, origin));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Light), (void*)offsetof(Light, direction));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Light), (void*)offsetof(Light, interval));
}

void clearShadow() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void updateLight(float time, bool isDay) {
	const float sunTime = -time / 100.0f;
	const float angle = sunTime + (float)M_PI_2 * 0.6f;

	sunPosition = (vec3){sinf(angle) + cosf(angle), cosf(angle) - sinf(angle), 0.0};
	lightPosition = isDay ? sunPosition : moonPosition;

	shadowView = viewMatrix(vec3_add(characterPosition, lightPosition), characterPosition, (vec3){0.0, 1.0, 0.0});
}

void renderLights(const mat4* restrict const view, float time) {
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(lightShader);

	glUniformMatrix4fv(glGetUniformLocation(lightShader, uniform_projection), 1, GL_FALSE, (GLfloat*)&projection);
	glUniformMatrix4fv(glGetUniformLocation(lightShader, uniform_view), 1, GL_FALSE, (GLfloat*)view);
	glUniform2fv(glGetUniformLocation(lightShader, uniform_resolution), 1, (GLfloat*)&screenSize);
	glUniform1f(glGetUniformLocation(lightShader, uniform_time), time);

	glBindVertexArray(pointLightVAO);
	glDrawArrays(GL_POINTS, 0, LIGHTS_NUMBER);

	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}

void cleanupShadow() {
	glDeleteFramebuffers(1, &shadowFBO);
	glDeleteTextures(1, &shadowMap);
	glDeleteBuffers(1, &pointLightSSBO);
	glDeleteVertexArrays(1, &pointLightVAO);
}