#include "shader.h"

#include <stdlib.h>
#ifdef DEBUG
	#include <stdio.h>
#endif

#include "resources.h"

static GLuint compileShaderStage(GLenum type, const char* code) {
	if (!code) return 0;

	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &code, NULL);
	glCompileShader(shader);

#ifdef DEBUG
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		printf("ERROR::SHADER::COMPILATION_FAILED (%d)\n%s\n", type, infoLog);
	}
#endif

	return shader;
}

static GLuint linkShaderProgram(GLuint* shaders, int count) {
	GLuint program = glCreateProgram();
	for (int i = 0; i < count; ++i) {
		if (shaders[i]) glAttachShader(program, shaders[i]);
	}
	glLinkProgram(program);

#ifdef DEBUG
	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		printf("ERROR::SHADER::LINK_FAILED\n%s\n", infoLog);
	}
#endif

	for (int i = 0; i < count; ++i) {
		if (shaders[i]) glDeleteShader(shaders[i]);
	}

	return program;
}

static GLuint compileShader(const char* vs, const char* tcs, const char* tes, const char* gs, const char* fs) {
	GLuint shaders[5];
	shaders[0] = compileShaderStage(GL_VERTEX_SHADER, vs);
	shaders[1] = compileShaderStage(GL_TESS_CONTROL_SHADER, tcs);
	shaders[2] = compileShaderStage(GL_TESS_EVALUATION_SHADER, tes);
	shaders[3] = compileShaderStage(GL_GEOMETRY_SHADER, gs);
	shaders[4] = compileShaderStage(GL_FRAGMENT_SHADER, fs);
	return linkShaderProgram(shaders, 5);
}

static GLuint compileComputeShader(const char* cs) {
	GLuint shader = compileShaderStage(GL_COMPUTE_SHADER, cs);
	return linkShaderProgram(&shader, 1);
}

GLuint basicShader;

GLuint textShader;
GLuint snoiseShader;
GLuint rnoiseShader;

GLuint atmosphereShader;
GLuint skyShader;
GLuint moonShader;

GLuint snowShader;
GLuint updateSnowShader;
GLuint iceShader;

GLuint terrainShader;
GLuint grassCompShader;
GLuint grassShader;

GLuint characterShader;
GLuint shadowCharacterShader;

GLuint treeShader;
GLuint shadowTreeShader;

GLuint lightShader;

GLuint transitionShader;

void initShaders() {
	const char *compressedShaders = (char*)res_shaders_pack.data;

	// Basic shaders
	char *basicVertSrc = getShaderSourceFromFile(compressedShaders, shader_basicVertSrc);
	char *basicFragSrc = getShaderSourceFromFile(compressedShaders, shader_basicFragSrc);

	basicShader = compileShader(basicVertSrc, NULL, NULL, NULL, basicFragSrc);

	// Text shaders
	char *textVertSrc = getShaderSourceFromFile(compressedShaders, shader_textVertSrc);
	char *textFragSrc = getShaderSourceFromFile(compressedShaders, shader_textFragSrc);

	textShader = compileShader(textVertSrc, NULL, NULL, NULL, textFragSrc);

	free(textVertSrc);
	free(textFragSrc);

	// Noise shaders
	char *postVertSrc = getShaderSourceFromFile(compressedShaders, shader_postVertSrc);
	char *snoiseFragSrc = getShaderSourceFromFile(compressedShaders, shader_snoiseFragSrc);
	char *rnoiseFragSrc = getShaderSourceFromFile(compressedShaders, shader_rnoiseFragSrc);

	snoiseShader = compileShader(postVertSrc, NULL, NULL, NULL, snoiseFragSrc);
	rnoiseShader = compileShader(postVertSrc, NULL, NULL, NULL, rnoiseFragSrc);

	free(snoiseFragSrc);
	free(rnoiseFragSrc);

	// Atmosphere shader
	char *atmosphereFragSrc = getShaderSourceFromFile(compressedShaders, shader_atmosphereFragSrc);

	atmosphereShader = compileShader(postVertSrc, NULL, NULL, NULL, atmosphereFragSrc);

	free(atmosphereFragSrc);

	// Sky shaders
	char *skyVertSrc = getShaderSourceFromFile(compressedShaders, shader_skyVertSrc);
	char *skyFragSrc = getShaderSourceFromFile(compressedShaders, shader_skyFragSrc);

	skyShader = compileShader(skyVertSrc, NULL, NULL, NULL, skyFragSrc);

	free(skyVertSrc);
	free(skyFragSrc);

	// Moon shaders
	char *moonVertSrc = getShaderSourceFromFile(compressedShaders, shader_moonVertSrc);
	char *moonFragSrc = getShaderSourceFromFile(compressedShaders, shader_moonFragSrc);

	moonShader = compileShader(moonVertSrc, NULL, NULL, NULL, moonFragSrc);

	free(moonVertSrc);
	free(moonFragSrc);

	// Snow shaders
	char *snowVertSrc = getShaderSourceFromFile(compressedShaders, shader_snowVertSrc);
	char *snowTCSSrc = getShaderSourceFromFile(compressedShaders, shader_snowTCSSrc);
	char *snowTESCSrc = getShaderSourceFromFile(compressedShaders, shader_snowTESCSrc);
	char *snowFragSrc = getShaderSourceFromFile(compressedShaders, shader_snowFragSrc);
	char *updateSnowFragSrc = getShaderSourceFromFile(compressedShaders, shader_updateSnowFragSrc);

	snowShader = compileShader(snowVertSrc, snowTCSSrc, snowTESCSrc, NULL, snowFragSrc);
	updateSnowShader = compileShader(postVertSrc, NULL, NULL, NULL, updateSnowFragSrc);

	free(snowVertSrc);
	free(snowTCSSrc);
	free(snowTESCSrc);
	free(snowFragSrc);
	free(updateSnowFragSrc);

	// Ice shaders
	char *iceFragSrc = getShaderSourceFromFile(compressedShaders, shader_iceFragSrc);

	iceShader = compileShader(basicVertSrc, NULL, NULL, NULL, iceFragSrc);

	free(iceFragSrc);

	// Terrain shaders
	char *terrainVertSrc = getShaderSourceFromFile(compressedShaders, shader_terrainVertSrc);
	char *terrainFragSrc = getShaderSourceFromFile(compressedShaders, shader_terrainFragSrc);

	terrainShader = compileShader(terrainVertSrc, NULL, NULL, NULL, terrainFragSrc);

	free(terrainVertSrc);
	free(terrainFragSrc);

	// Grass shaders
	char *grassCompSrc = getShaderSourceFromFile(compressedShaders, shader_grassCompSrc);
	char *grassVertSrc = getShaderSourceFromFile(compressedShaders, shader_grassVertSrc);
	char *grassFragSrc = getShaderSourceFromFile(compressedShaders, shader_grassFragSrc);

	grassCompShader = compileComputeShader(grassCompSrc);
	grassShader = compileShader(grassVertSrc, NULL, NULL, NULL, grassFragSrc);

	free(grassCompSrc);
	free(grassVertSrc);
	free(grassFragSrc);

	// Character shaders
	char *characterVertSrc = getShaderSourceFromFile(compressedShaders, shader_characterVertSrc);
	char *characterFragSrc = getShaderSourceFromFile(compressedShaders, shader_characterFragSrc);

	characterShader = compileShader(characterVertSrc, NULL, NULL, NULL, characterFragSrc);
	shadowCharacterShader = compileShader(characterVertSrc, NULL, NULL, NULL, basicFragSrc);

	free(characterVertSrc);
	free(characterFragSrc);

	// Tree shaders
	char *treeVertSrc = getShaderSourceFromFile(compressedShaders, shader_treeVertSrc);
	char *treeFragSrc = getShaderSourceFromFile(compressedShaders, shader_treeFragSrc);

	treeShader = compileShader(treeVertSrc, NULL, NULL, NULL, treeFragSrc);
	shadowTreeShader = compileShader(treeVertSrc, NULL, NULL, NULL, basicFragSrc);

	free(treeVertSrc);
	free(treeFragSrc);

	// Light shaders
	char *lightVertSrc = getShaderSourceFromFile(compressedShaders, shader_lightVertSrc);
	char *lightFragSrc = getShaderSourceFromFile(compressedShaders, shader_lightFragSrc);

	lightShader = compileShader(lightVertSrc, NULL, NULL, NULL, lightFragSrc);

	free(lightVertSrc);
	free(lightFragSrc);

	// Transition shaders

	char *transitionFragSrc = getShaderSourceFromFile(compressedShaders, shader_transitionFragSrc);

	transitionShader = compileShader(postVertSrc, NULL, NULL, NULL, transitionFragSrc);

	free(transitionFragSrc);


	// Cleanup
	free(basicVertSrc);
	free(basicFragSrc);
	free(postVertSrc);
}

void cleanupShaders() {
	glDeleteProgram(basicShader);

	glDeleteProgram(textShader);
	glDeleteProgram(snoiseShader);
	glDeleteProgram(rnoiseShader);

	glDeleteProgram(atmosphereShader);
	glDeleteProgram(skyShader);
	glDeleteProgram(moonShader);

	glDeleteProgram(snowShader);
	glDeleteProgram(updateSnowShader);
	glDeleteProgram(iceShader);

	glDeleteProgram(terrainShader);

	glDeleteProgram(characterShader);
	glDeleteProgram(shadowCharacterShader);

	glDeleteProgram(treeShader);
	glDeleteProgram(shadowTreeShader);

	glDeleteProgram(lightShader);
}