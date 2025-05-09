#include "grass.h"

#include "glutils.h"
#include "cameraController.h"

#define GRASS_CHUNKS 2
#define GRASS_PER_CHUNK 500000
#define GRASS_AREA 10.0f
#define HEIGHT_SIZE 2048

static mat4 model;
static Mesh ground;

static GLuint grassVBO, grassVAO;
static GLuint grassMeshVBO, grassMeshVAO;

static GLuint terrainHeightMap;

static const vec3 vertices[] = {
	{ 0.0f, 0.000f, -0.052f },
	{ 0.0f, 0.000f,  0.052f },
	{ 0.0f, 0.131f, -0.046f },
	{ 0.0f, 0.131f,  0.046f },
	{ 0.0f, 0.277f, -0.033f },
	{ 0.0f, 0.277f,  0.033f },
	{ 0.0f, 0.407f, -0.017f },
	{ 0.0f, 0.407f,  0.018f },
	{ 0.0f, 0.508f,  0.000f }
};

void initGrass() {
	model = rotationMatrix((vec3){0.0f, -(float)M_PI / 6.0f, 0.0f});

	ground = generateGrid((vec2){GRASS_AREA, GRASS_AREA}, 10, 0.0f);

	terrainHeightMap = createTexture(HEIGHT_SIZE, HEIGHT_SIZE);
	GLuint terrainHeightMapFBO = createFramebuffer(terrainHeightMap);

	glBindFramebuffer(GL_FRAMEBUFFER, terrainHeightMapFBO);
	glViewport(0, 0, HEIGHT_SIZE, HEIGHT_SIZE);

	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(snoiseShader);

	float size = (float)HEIGHT_SIZE;
	glUniform2f(glGetUniformLocation(snoiseShader, uniform_resolution), size, size);
	glUniform2f(glGetUniformLocation(snoiseShader, uniform_offset), 0.05f, -0.05f);

	renderScreenQuad();

	glDeleteFramebuffers(1, &terrainHeightMapFBO);


	// Create the grass mesh VAO
	glGenVertexArrays(1, &grassMeshVAO);
	glBindVertexArray(grassMeshVAO);

	glGenBuffers(1, &grassMeshVBO);
	glBindBuffer(GL_ARRAY_BUFFER, grassMeshVBO);

	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(vec3), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

	glGenBuffers(1, &grassVBO);
	glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * GRASS_PER_CHUNK, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, grassVBO);


	// Generating grass positions using compute shader
	glUseProgram(grassCompShader);
	glUniform1ui(glGetUniformLocation(grassCompShader, uniform_instanceCount), GRASS_PER_CHUNK);
	glUniform1f(glGetUniformLocation(grassCompShader, uniform_areaSize), GRASS_AREA);
	glDispatchCompute((GRASS_PER_CHUNK + 63) / 64, 1, 1);
	glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);


	// Create the final grass VAO
	glGenVertexArrays(1, &grassVAO);
	glBindVertexArray(grassVAO);

	glBindBuffer(GL_ARRAY_BUFFER, grassMeshVBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vec4), (void*)0);
	glVertexAttribDivisor(1, 1);
}

void renderGrass(float time) {
	for (int x = 0; x < GRASS_CHUNKS; x++) {
		for (int z = 0; z < GRASS_CHUNKS; z++) {
			vec2 offset = { (float)x / (float)GRASS_CHUNKS, (float)z / (float)GRASS_CHUNKS };

			glUseProgram(grassShader);

			glUniformMatrix4fv(glGetUniformLocation(grassShader, uniform_projection), 1, GL_FALSE, (const float*)&projection);
			glUniformMatrix4fv(glGetUniformLocation(grassShader, uniform_view), 1, GL_FALSE, (const float*)&cameraView);
			glUniformMatrix4fv(glGetUniformLocation(grassShader, uniform_model), 1, GL_FALSE, (const float*)&model);
			glUniform2fv(glGetUniformLocation(grassShader, uniform_offset), 1, (const float*)&offset);
			glUniform1f(glGetUniformLocation(grassShader, uniform_time), time);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, terrainHeightMap);
			glUniform1i(glGetUniformLocation(grassShader, uniform_heightMap), 0);

			glBindVertexArray(grassVAO);
			glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 9, GRASS_PER_CHUNK);


			glUseProgram(terrainShader);

			glUniformMatrix4fv(glGetUniformLocation(terrainShader, uniform_projection), 1, GL_FALSE, (const float*)&projection);
			glUniformMatrix4fv(glGetUniformLocation(terrainShader, uniform_view), 1, GL_FALSE, (const float*)&cameraView);
			glUniformMatrix4fv(glGetUniformLocation(terrainShader, uniform_model), 1, GL_FALSE, (const float*)&model);
			glUniform2fv(glGetUniformLocation(terrainShader, uniform_offset), 1, (const float*)&offset);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, terrainHeightMap);
			glUniform1i(glGetUniformLocation(terrainShader, uniform_heightMap), 0);

			glBindVertexArray(ground.VAO);
			glDrawElements(GL_TRIANGLES, ground.indexCount, GL_UNSIGNED_INT, 0);
		}
	}
}

void cleanupGrass() {
	freeMesh(ground);
	glDeleteTextures(1, &terrainHeightMap);
	glDeleteBuffers(1, &grassVBO);
	glDeleteBuffers(1, &grassMeshVBO);
	glDeleteVertexArrays(1, &grassVAO);
	glDeleteVertexArrays(1, &grassMeshVAO);
}