#include "snow.h"

#define CHUNK_RESOLUTION 1024

#define CHUNK_NBR_X 3

GLuint reflectionFrameBuffer = 0;
static GLuint reflectionTexture = 0;
static GLuint depthStencilTexture = 0;

static const float iceHeight = 0.35f;
static mat4 iceModel = {0};
static vec2 texturesSize = {0};

static const float mapCenterX = (CHUNK_NBR_X - 1) / 2.0f - 0.5f;
static const float mapCenterZ = (CHUNK_NBR_Z - 1) / 2.0f;

static GLuint heightmapTextureArray = 0;
static Mesh terrainMesh;
static mat4 terrainModel = {0};
static GLuint instanceVBO = 0;

static GLuint depthFBOs[2];
static GLuint depthTextures[2];
static int activeTexture = 0;

static vec2 currentPosition = {0.0f, 0.0f};
static mat4 updateProjection;

static Mesh generateGrid(vec2 size, int subdivision, float yOffset) {
	const int width = subdivision + 1;
	const int vertexNbr = (subdivision + 2) * (subdivision + 2);
	const int indexNbr = width * width * 6;

	GLuint vao = 0;
	vec3 *positions = (vec3*)malloc(sizeof(vec3) * vertexNbr);
	unsigned int *indices = (unsigned int*)malloc(sizeof(int) * indexNbr);

	if (positions && indices) {
		int index = 0;

		for (int y = 0; y <= width; y++) {
			for (int x = 0; x <= width; x++) {
				float posX = size.x * ((float)x / (float)width);
				float posZ = size.y * ((float)y / (float)width);

				positions[x + y * (width + 1)] = (vec3){posX - size.x / 2.0f, yOffset, posZ - size.y / 2.0f};

				if (x < width && y < width) {
					int topLeft = x + y * (width + 1);
					int topRight = (x + 1) + y * (width + 1);
					int bottomLeft = x + (y + 1) * (width + 1);
					int bottomRight = (x + 1) + (y + 1) * (width + 1);

					// First triangle
					indices[index++] = topLeft;
					indices[index++] = bottomLeft;
					indices[index++] = topRight;

					// Second triangle
					indices[index++] = topRight;
					indices[index++] = bottomLeft;
					indices[index++] = bottomRight;
				}
			}
		}

		vao = createIndexedVAO(positions, vertexNbr, indices, indexNbr);
		free(positions);
		free(indices);
	}

	return (Mesh){vao, vertexNbr, indexNbr};
}

void initSnow() {
	texturesSize = screenSize;

	terrainModel = translationMatrix((vec3){3.0, 0.0, CHUNK_NBR_Z * CHUNK_SIZE / 2.0f - CHUNK_SIZE});
	terrainMesh = generateGrid((vec2){CHUNK_SIZE, CHUNK_SIZE}, 25, 0.0f);

	glGenTextures(1, &heightmapTextureArray);
	glBindTexture(GL_TEXTURE_2D_ARRAY, heightmapTextureArray);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGB8, CHUNK_RESOLUTION, CHUNK_RESOLUTION, CHUNK_NBR_X * CHUNK_NBR_Z);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLuint framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glViewport(0, 0, CHUNK_RESOLUTION, CHUNK_RESOLUTION);

	const int instanceCount = CHUNK_NBR_X * CHUNK_NBR_Z;
	vec3 *offsets = (vec3*)malloc(sizeof(vec3) * instanceCount);
	int instanceIndex = 0;
	for (int z = 0; z < CHUNK_NBR_Z; z++) {
		for (int x = 0; x < CHUNK_NBR_X; x++) {
			glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, heightmapTextureArray, 0, instanceIndex);
			glClear(GL_COLOR_BUFFER_BIT);

			glUseProgram(snoiseShader);
			glUniform2f(glGetUniformLocation(snoiseShader, uniform_resolution), CHUNK_RESOLUTION, CHUNK_RESOLUTION);
			glUniform2f(glGetUniformLocation(snoiseShader, uniform_offset), (float)x, (float)z);
			renderScreenQuad();

			offsets[instanceIndex] = (vec3){
				((float)x - mapCenterX) * CHUNK_SIZE,
				(float)instanceIndex,
				((float)z - mapCenterZ) * CHUNK_SIZE
			};

			instanceIndex++;
		}
	}

	instanceVBO = setupInstanceBuffer(terrainMesh.VAO, offsets, instanceCount);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &framebuffer);
	free(offsets);


	depthTextures[0] = createTextureDepth(CHUNK_RESOLUTION, CHUNK_RESOLUTION);
	depthTextures[1] = createTextureDepth(CHUNK_RESOLUTION, CHUNK_RESOLUTION);

	depthFBOs[0] = createFramebufferDepth(depthTextures[0]);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBOs[0]);
	glClear(GL_DEPTH_BUFFER_BIT);
	depthFBOs[1] = createFramebufferDepth(depthTextures[1]);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBOs[1]);
	glClear(GL_DEPTH_BUFFER_BIT);

	updateProjection = orthographicMatrix(-CHUNK_SIZE, CHUNK_SIZE, -CHUNK_SIZE, CHUNK_SIZE, 0.0, 1.0);


	reflectionTexture = createTexture((int)texturesSize.x, (int)texturesSize.y);
	depthStencilTexture = createTextureStencil((int)texturesSize.x, (int)texturesSize.y);

	reflectionFrameBuffer = createFramebufferDepthStencil(depthStencilTexture, reflectionTexture);

	iceModel = rotationMatrix((vec3){(float)M_PI / 2.0f, 0.0f, 0.0f});
	iceModel.m[3][1] = iceHeight;
}

mat4 updateSnow(vec3 *reflectionDirection, const mat4 *projection, const mat4 *characterModel) {
	if (texturesSize.x != screenSize.x || texturesSize.y != screenSize.y) {
		texturesSize = screenSize;

		glDeleteTextures(1, &reflectionTexture);
		glDeleteTextures(1, &depthStencilTexture);
		glDeleteFramebuffers(1, &reflectionFrameBuffer);

		reflectionTexture = createTexture((int)texturesSize.x, (int)texturesSize.y);
		depthStencilTexture = createTextureStencil((int)texturesSize.x, (int)texturesSize.y);

		reflectionFrameBuffer = createFramebufferDepthStencil(depthStencilTexture, reflectionTexture);
	}

	mat4 reflectionView = reflectionCameraMatrix(reflectionDirection, &(vec3){0.0f, 1.0f, 0.0f}, iceHeight);

	vec2 nextPosition = {characterPosition.x, characterPosition.z};
	mat4 updateView = viewMatrix((vec3){nextPosition.x, 0.0f, nextPosition.y}, (vec3){nextPosition.x, 1.0f, nextPosition.y}, (vec3){0.0f, 0.0f, 1.0f});

	vec2 offset = vec2_subtract(nextPosition, currentPosition);
	offset = vec2_scale(offset, 0.5f / CHUNK_SIZE);

	int nextTexture = (activeTexture + 1) % 2;

	glBindFramebuffer(GL_FRAMEBUFFER, depthFBOs[nextTexture]);
	glViewport(0, 0, CHUNK_RESOLUTION, CHUNK_RESOLUTION);

	glUseProgram(updateSnowShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTextures[activeTexture]);
	glUniform1i(glGetUniformLocation(updateSnowShader, uniform_previousDepthMap), 0);

	glUniform2fv(glGetUniformLocation(updateSnowShader, uniform_offset), 1, (GLfloat*)&offset);

	glDepthFunc(GL_ALWAYS);
	renderScreenQuad();
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glDepthFunc(GL_LESS);

	renderCharacter(shadowCharacterShader, &updateProjection, &updateView, characterModel);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	activeTexture = nextTexture;
	currentPosition = nextPosition;


	glBindFramebuffer(GL_FRAMEBUFFER, reflectionFrameBuffer);
	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glEnable(GL_STENCIL_TEST);

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glUseProgram(basicShader);

	glUniformMatrix4fv(glGetUniformLocation(basicShader, uniform_projection), 1, GL_FALSE, (GLfloat*)&projection);
	glUniformMatrix4fv(glGetUniformLocation(basicShader, uniform_view), 1, GL_FALSE, (GLfloat*)&reflectionView);
	glUniformMatrix4fv(glGetUniformLocation(basicShader, uniform_model), 1, GL_FALSE, (GLfloat*)&iceModel);

	renderScreenQuad();

	glDisable(GL_STENCIL_TEST);

	return reflectionView;
}

void renderSnow(const mat4 *projection, const mat4 *view, const mat4 *reflectionView, int chunkZ) {
	glUseProgram(snowShader);

	glUniformMatrix4fv(glGetUniformLocation(snowShader, uniform_projection), 1, GL_FALSE, (GLfloat*)projection);
	glUniformMatrix4fv(glGetUniformLocation(snowShader, uniform_view), 1, GL_FALSE, (GLfloat*)view);
	glUniformMatrix4fv(glGetUniformLocation(snowShader, uniform_model), 1, GL_FALSE, (GLfloat*)&terrainModel);
	glUniformMatrix4fv(glGetUniformLocation(snowShader, uniform_shadowProjection), 1, GL_FALSE, (GLfloat*)&shadowProjection);
	glUniformMatrix4fv(glGetUniformLocation(snowShader, uniform_shadowView), 1, GL_FALSE, (GLfloat*)&shadowView);
	glUniform3fv(glGetUniformLocation(snowShader, uniform_lightPos), 1, (GLfloat*)&lightPosition);
	glUniform3fv(glGetUniformLocation(snowShader, uniform_viewPos), 1, (GLfloat*)&cameraPos);
	glUniform2f(glGetUniformLocation(snowShader, uniform_characterPos), currentPosition.x, currentPosition.y);
	glUniform1f(glGetUniformLocation(snowShader, uniform_size), CHUNK_SIZE);
	glUniform2ui(glGetUniformLocation(snowShader, uniform_chunks), CHUNK_NBR_X, CHUNK_NBR_Z);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glUniform1i(glGetUniformLocation(snowShader, uniform_shadowMap), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthTextures[activeTexture]);
	glUniform1i(glGetUniformLocation(snowShader, uniform_heightTex), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D_ARRAY, heightmapTextureArray);
	glUniform1i(glGetUniformLocation(snowShader, uniform_heightmapArray), 2);

	int offset = chunkZ * CHUNK_NBR_X;
	int limit = CHUNK_NBR_X * CHUNK_NBR_Z - offset ;

	glPatchParameteri(GL_PATCH_VERTICES, 3);
	glBindVertexArray(terrainMesh.VAO);

	glUniform1f(glGetUniformLocation(snowShader, uniform_worldZOffset), currentZOffset);
	glDrawElementsInstancedBaseInstance(GL_PATCHES, terrainMesh.indexCount, GL_UNSIGNED_INT, 0, limit, offset);

	glUniform1f(glGetUniformLocation(snowShader, uniform_worldZOffset), currentZOffset + CHUNK_NBR_Z * CHUNK_SIZE);
	glDrawElementsInstancedBaseInstance(GL_PATCHES, terrainMesh.indexCount, GL_UNSIGNED_INT, 0, offset, 0);

	glBindVertexArray(0);
	glUseProgram(0);

	glUseProgram(iceShader);

	glUniformMatrix4fv(glGetUniformLocation(iceShader, uniform_projection), 1, GL_FALSE, (GLfloat*)projection);
	glUniformMatrix4fv(glGetUniformLocation(iceShader, uniform_view), 1, GL_FALSE, (GLfloat*)view);
	glUniformMatrix4fv(glGetUniformLocation(iceShader, uniform_reflectionView), 1, GL_FALSE, (GLfloat*)reflectionView);
	glUniformMatrix4fv(glGetUniformLocation(iceShader, uniform_model), 1, GL_FALSE, (GLfloat*)&iceModel);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, reflectionTexture);
	glUniform1i(glGetUniformLocation(iceShader, uniform_reflection), 0);

	renderScreenQuad();
}

void cleanupSnow() {
	freeMesh(terrainMesh);

	glDeleteBuffers(1, &instanceVBO);
	glDeleteTextures(1, &heightmapTextureArray);

	glDeleteTextures(2, depthTextures);
	glDeleteFramebuffers(2, depthFBOs);

	glDeleteFramebuffers(1, &reflectionFrameBuffer);
	glDeleteTextures(1, &reflectionTexture);
	glDeleteTextures(1, &depthStencilTexture);
}