#include "sky.h"

static GLuint skyboxVAO;
static GLuint skyboxTexture;
static GLuint captureFBO, captureRBO;

static vec3 vertices[] = {
    {-1.0f,  1.0f, -1.0f},
    { 1.0f,  1.0f, -1.0f},
    { 1.0f, -1.0f, -1.0f},
    {-1.0f, -1.0f, -1.0f},
    {-1.0f,  1.0f,  1.0f},
    { 1.0f,  1.0f,  1.0f},
    { 1.0f, -1.0f,  1.0f},
    {-1.0f, -1.0f,  1.0f}
};

static unsigned int indices[] = {
    0, 2, 1,   0, 3, 2,
    4, 5, 6,   4, 6, 7,
    0, 1, 5,   0, 5, 4,
    3, 6, 2,   3, 7, 6,
    3, 4, 7,   3, 0, 4,
    1, 2, 6,   1, 6, 5
};

#define SKYBOX_RESOLUTION 2048

void initSky() {
	skyboxVAO = createIndexedVAO(vertices, 8, indices, 36);
	skyboxTexture = createCubeMap(SKYBOX_RESOLUTION, SKYBOX_RESOLUTION);

	glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, SKYBOX_RESOLUTION, SKYBOX_RESOLUTION);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void updateSky(const vec3* sunPosition, const vec2* screenSize, float ftime, const vec3* updateDirection) {
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glViewport(0, 0, SKYBOX_RESOLUTION, SKYBOX_RESOLUTION);

	for (unsigned int i = 0; i < 6; i++) {
		if (i == 3) continue;

		vec3 normal = {0.0f, 0.0f, 0.0f};
		((float*)&normal)[i / 2] = (i % 2 == 0 ? 1.0f : -1.0f);

		if (vec3_dot(cameraDirection, normal) <= 0.0f && vec3_dot(*updateDirection, normal) <= 0.0f) continue;

		mat4 view = viewMatrix((vec3){0.0f, 0.0f, 0.0f}, vec3_scale(normal, i <= 1 ? -1.0f : 1.0f), i == 2 ? (vec3){0.0f, 0.0f, 1.0f} : (vec3){0.0f, -1.0f, 0.0f});

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, skyboxTexture, 0);

		glClear(GL_DEPTH_BUFFER_BIT);

		glUseProgram(atmosphereShader);
		glUniformMatrix4fv(glGetUniformLocation(atmosphereShader, "view"), 1, GL_FALSE, (GLfloat*)&view);
		glUniform3fv(glGetUniformLocation(atmosphereShader, "sunPosition"), 1, (GLfloat*)sunPosition);
		glUniform2f(glGetUniformLocation(atmosphereShader, "iResolution"), SKYBOX_RESOLUTION, SKYBOX_RESOLUTION);
		glUniform1f(glGetUniformLocation(atmosphereShader, "iTime"), ftime);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rnoiseTexture);
		glUniform1i(glGetUniformLocation(atmosphereShader, "rnoise"), 0);
		
		renderScreenQuad();
	}

	glViewport(0, 0, screenSize->x, screenSize->y);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderSky(const mat4* projection, const mat4* view) {
	glDepthFunc(GL_LEQUAL);

	glUseProgram(skyShader);

	glUniformMatrix4fv(glGetUniformLocation(skyShader, "projection"), 1, GL_FALSE, (GLfloat*)projection);
	glUniformMatrix4fv(glGetUniformLocation(skyShader, "view"), 1, GL_FALSE, (GLfloat*)view);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	glUniform1i(glGetUniformLocation(skyShader, "skybox"), 0);

	glBindVertexArray(skyboxVAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glDepthFunc(GL_LESS);
}

void cleanupSky() {
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteTextures(1, &skyboxTexture);
    glDeleteFramebuffers(1, &captureFBO);
    glDeleteRenderbuffers(1, &captureRBO);
}