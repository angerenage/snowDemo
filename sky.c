#include "sky.h"

static GLuint skyboxVAO;
static GLuint skyboxTexture;
static GLuint captureFBO;

static GLuint moonTexture;

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
	3, 4, 7,   3, 0, 4,
	1, 2, 6,   1, 6, 5
};

#define SKYBOX_RESOLUTION 2048

void initSky() {
	skyboxVAO = createIndexedVAO(vertices, 8, indices, 30);
	skyboxTexture = createCubeMap(SKYBOX_RESOLUTION, SKYBOX_RESOLUTION);

	glGenFramebuffers(1, &captureFBO);


	GLuint moonVAO, moonVBO;

	glGenVertexArrays(1, &moonVAO);
	glBindVertexArray(moonVAO);

	glGenBuffers(1, &moonVBO);
	glBindBuffer(GL_ARRAY_BUFFER, moonVBO);

	glBufferData(GL_ARRAY_BUFFER, res_moon_circles.size, res_moon_circles.data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(uint8_t) * 4, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(uint8_t) * 4, (void*)(sizeof(uint8_t) * 2));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(uint8_t) * 4, (void*)(sizeof(uint8_t) * 3));

	moonTexture = createTexture(256, 256);
	GLuint moonFBO = createFramebuffer(moonTexture);

	glBindFramebuffer(GL_FRAMEBUFFER, moonFBO);
	glViewport(0, 0, 256, 256);

	glUseProgram(moonShader);

	glBindVertexArray(moonVAO);
	glDrawArrays(GL_POINTS, 0, (GLsizei)res_moon_circles.size / (sizeof(uint8_t) * 4));

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDeleteVertexArrays(1, &moonVAO);
	glDeleteBuffers(1, &moonVBO);
	glDeleteFramebuffers(1, &moonFBO);
}

void updateSky(float ftime, bool isDay, const vec3* updateDirection) {
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glViewport(0, 0, SKYBOX_RESOLUTION, SKYBOX_RESOLUTION);

	vec3 sunPosition = {0.0f, -1.0f, 0.0f};
	vec3 moonPosition = {0.0f, -1.0f, 0.0f};

	if (isDay) sunPosition = lightPosition;
	else moonPosition = (vec3){lightPosition.x, lightPosition.y, lightPosition.z * -1.0f};

	glUseProgram(atmosphereShader);

	glUniform3fv(glGetUniformLocation(atmosphereShader, uniform_sunPosition), 1, (GLfloat*)&sunPosition);
	glUniform3fv(glGetUniformLocation(atmosphereShader, uniform_moonPosition), 1, (GLfloat*)&moonPosition);

	glUniform2f(glGetUniformLocation(atmosphereShader, uniform_resolution), SKYBOX_RESOLUTION, SKYBOX_RESOLUTION);
	glUniform1f(glGetUniformLocation(atmosphereShader, uniform_time), ftime);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, rnoiseTexture);
	glUniform1i(glGetUniformLocation(atmosphereShader, uniform_noiseTex), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, moonTexture);
	glUniform1i(glGetUniformLocation(atmosphereShader, uniform_moonTex), 1);

	for (unsigned int i = 0; i < 6; i++) {
		if (i == 3) continue;

		vec3 normal = {0.0f, 0.0f, 0.0f};
		((float*)&normal)[i / 2] = (i % 2 == 0 ? 1.0f : -1.0f);

		bool cameraLooks = vec3_dot(vec3_normalize(cameraDirection), normal) > 0.0f;
		bool reflectionLooks = updateDirection ? vec3_dot(vec3_normalize(*updateDirection), normal) > 0.0f : false;
		if (!cameraLooks && !reflectionLooks) continue;

		mat4 view = viewMatrix((vec3){0.0f, 0.0f, 0.0f}, vec3_scale(normal, i <= 1 ? -1.0f : 1.0f), i == 2 ? (vec3){0.0f, 0.0f, 1.0f} : (vec3){0.0f, -1.0f, 0.0f});

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, skyboxTexture, 0);

		glClear(GL_DEPTH_BUFFER_BIT);

		glUniformMatrix4fv(glGetUniformLocation(atmosphereShader, uniform_view), 1, GL_FALSE, (GLfloat*)&view);
		
		renderScreenQuad();
	}

	glViewport(0, 0, (GLsizei)screenSize.x, (GLsizei)screenSize.y);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderSky(const mat4* projection, const mat4* view) {
	glDepthFunc(GL_LEQUAL);

	glUseProgram(skyShader);

	glUniformMatrix4fv(glGetUniformLocation(skyShader, uniform_projection), 1, GL_FALSE, (GLfloat*)projection);
	glUniformMatrix4fv(glGetUniformLocation(skyShader, uniform_view), 1, GL_FALSE, (GLfloat*)view);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	glUniform1i(glGetUniformLocation(skyShader, uniform_skybox), 0);

	glBindVertexArray(skyboxVAO);
	glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glDepthFunc(GL_LESS);
}

void cleanupSky() {
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteTextures(1, &skyboxTexture);
	glDeleteFramebuffers(1, &captureFBO);
	glDeleteTextures(1, &moonTexture);
}