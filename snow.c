#include "snow.h"

#define TERRAIN_RESOLUTION 4096

Mesh generateGrid(vec2 size, int subdivision, float yOffset) {
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

GLuint generateTerrainHeight(const vec2 *pos) {
	GLuint terrainHeight = createTexture(TERRAIN_RESOLUTION, TERRAIN_RESOLUTION);
	GLuint noiseFBO = createFramebuffer(terrainHeight);

	glViewport(0, 0, TERRAIN_RESOLUTION, TERRAIN_RESOLUTION);
	glBindFramebuffer(GL_FRAMEBUFFER, noiseFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(snoiseShader);

	glUniform2f(glGetUniformLocation(snoiseShader, "resolution"), TERRAIN_RESOLUTION, TERRAIN_RESOLUTION);
	glUniform2fv(glGetUniformLocation(snoiseShader, "offset"), 1, pos);

	renderScreenQuad();

	glDeleteFramebuffers(1, &noiseFBO);

	return terrainHeight;
}