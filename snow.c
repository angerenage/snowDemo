#include "snow.h"

#define TERRAIN_RESOLUTION 4096

#define CHUNK_NBR_X 2
#define CHUNK_NBR_Z 3

static const float chunkSize = 10.0f;
static const float mapCenterX = (CHUNK_NBR_X - 1) / 2.0f - 0.5f;
static const float mapCenterZ = (CHUNK_NBR_Z - 1) / 2.0f;

static GLuint terrainHeights[CHUNK_NBR_X][CHUNK_NBR_Z];
static Mesh terrainMesh;

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

				positions[x + y * (width + 1)] = (vec3){posX - size.x / 2.0, yOffset, posZ - size.y / 2.0};

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

static GLuint generateTerrainHeight(const vec2 *pos) {
	GLuint terrainHeight = createTexture(TERRAIN_RESOLUTION, TERRAIN_RESOLUTION);
	GLuint noiseFBO = createFramebuffer(terrainHeight);

	glViewport(0, 0, TERRAIN_RESOLUTION, TERRAIN_RESOLUTION);
	glBindFramebuffer(GL_FRAMEBUFFER, noiseFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(snoiseShader);

	glUniform2f(glGetUniformLocation(snoiseShader, "resolution"), TERRAIN_RESOLUTION, TERRAIN_RESOLUTION);
	glUniform2fv(glGetUniformLocation(snoiseShader, "offset"), 1, (GLfloat*)pos);

	renderScreenQuad();

	glDeleteFramebuffers(1, &noiseFBO);

	return terrainHeight;
}

void initSnow() {
	terrainMesh = generateGrid((vec2){chunkSize, chunkSize}, 100, 0.3f);

	for (int x = 0; x < CHUNK_NBR_X; x++) {
		for (int z = 0; z < CHUNK_NBR_Z; z++) {
			terrainHeights[x][z] = generateTerrainHeight(&(vec2){(float)x, (float)z});
		}
	}
}

void renderSnow(GLuint shader, const mat4 *projection, const mat4 *view) {
	glUseProgram(shader);

	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, (GLfloat*)projection);
	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, (GLfloat*)view);
	glUniformMatrix4fv(glGetUniformLocation(shader, "shadowProjection"), 1, GL_FALSE, (GLfloat*)&shadowProjection);
	glUniformMatrix4fv(glGetUniformLocation(shader, "shadowView"), 1, GL_FALSE, (GLfloat*)&shadowView);
	glUniform3fv(glGetUniformLocation(shader, "sunPos"), 1, (GLfloat*)&sunPosition);
	glUniform3fv(glGetUniformLocation(shader, "viewPos"), 1, (GLfloat*)&cameraPos);
	glUniform1f(glGetUniformLocation(shader, "size"), chunkSize);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glUniform1i(glGetUniformLocation(shader, "shadowMap"), 0);

	glBindVertexArray(terrainMesh.VAO);
	for (int x = 0; x < CHUNK_NBR_X; x++) {
		for (int z = 0; z < CHUNK_NBR_Z; z++) {
			mat4 model = translationMatrix((vec3){((float)x - mapCenterX) * chunkSize, 0.0, ((float)z - mapCenterZ) * chunkSize});

			glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (GLfloat*)&model);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, terrainHeights[x][z]);
			glUniform1i(glGetUniformLocation(shader, "heightTex"), 1);

			glDrawElements(GL_TRIANGLES, terrainMesh.indexCount, GL_UNSIGNED_INT, 0);
		}
	}

	glBindVertexArray(0);
	glUseProgram(0);
}

void cleanupSnow() {
	freeMesh(terrainMesh);

	for (int i = 0; i < CHUNK_NBR_X; i++) {
		glDeleteTextures(CHUNK_NBR_Z, terrainHeights[i]);
	}
}