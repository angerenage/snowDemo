#include "shader.h"

static GLuint compileShader(const char* const vShaderCode, const char* const tcsShaderCode, const char* const tesShaderCode, const char* const gShaderCode, const char* const fShaderCode) {
	GLuint vertex = 0, tcs = 0, tes = 0, geometry = 0, fragment = 0;

#ifdef DEBUG
	int success;
	char infoLog[512];
#endif

	// Vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);

#ifdef DEBUG
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
	}
#endif

	// Tessellation Control Shader (TCS)
	if (tcsShaderCode) {
		tcs = glCreateShader(GL_TESS_CONTROL_SHADER);
		glShaderSource(tcs, 1, &tcsShaderCode, NULL);
		glCompileShader(tcs);

#ifdef DEBUG
		glGetShaderiv(tcs, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(tcs, 512, NULL, infoLog);
			printf("ERROR::SHADER::TESS_CONTROL::COMPILATION_FAILED\n%s\n", infoLog);
		}
#endif
	}

	// Tessellation Evaluation Shader (TES)
	if (tesShaderCode) {
		tes = glCreateShader(GL_TESS_EVALUATION_SHADER);
		glShaderSource(tes, 1, &tesShaderCode, NULL);
		glCompileShader(tes);

#ifdef DEBUG
		glGetShaderiv(tes, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(tes, 512, NULL, infoLog);
			printf("ERROR::SHADER::TESS_EVALUATION::COMPILATION_FAILED\n%s\n", infoLog);
		}
#endif
	}

	// Geometry shader
	if (gShaderCode) {
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);

#ifdef DEBUG
		glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(geometry, 512, NULL, infoLog);
			printf("ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n%s\n", infoLog);
		}
#endif
	}

	// Fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

#ifdef DEBUG
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
	}
#endif

	// Shader program
	GLuint ID = glCreateProgram();
	glAttachShader(ID, vertex);
	if (tcsShaderCode) glAttachShader(ID, tcs);
	if (tesShaderCode) glAttachShader(ID, tes);
	if (gShaderCode) glAttachShader(ID, geometry);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);

#ifdef DEBUG
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
	}
#endif

	// Clean up shaders
	glDeleteShader(vertex);
	if (tcsShaderCode) glDeleteShader(tcs);
	if (tesShaderCode) glDeleteShader(tes);
	if (gShaderCode) glDeleteShader(geometry);
	glDeleteShader(fragment);

	return ID;
}

static GLuint compileComputeShader(const char* const shaderCode) {
	GLuint compute;

#ifdef DEBUG
	int success;
	char infoLog[512];
#endif

	// compute shader
	compute = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(compute, 1, &shaderCode, NULL);
	glCompileShader(compute);
	
#ifdef DEBUG
	glGetShaderiv(compute, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(compute, 512, NULL, infoLog);
		printf("ERROR::SHADER::COMPUTE::COMPILATION_FAILED\n%s\n", infoLog);
	}
#endif

	GLuint ID = glCreateProgram();
	glAttachShader(ID, compute);
	glLinkProgram(ID);
	
#ifdef DEBUG
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
	}
#endif

	glDeleteShader(compute);

	return ID;
}

GLuint debugShader;
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

GLuint characterShader;
GLuint shadowCharacterShader;

GLuint treeShader;
GLuint shadowTreeShader;

GLuint lightShader;

void initShaders() {
	const char *compressedShaders = (char*)res_shaders_pack.data;

	// Basic shaders
	char *basicVertSrc = getShaderSourceFromFile(compressedShaders, shader_basicVertSrc);
	char *basicFragSrc = getShaderSourceFromFile(compressedShaders, shader_basicFragSrc);

	basicShader = compileShader(basicVertSrc, NULL, NULL, NULL, basicFragSrc);

	// Debug shaders
	char *debugFragSrc = getShaderSourceFromFile(compressedShaders, shader_debugFragSrc);

	debugShader = compileShader(basicVertSrc, NULL, NULL, NULL, debugFragSrc);

	free(debugFragSrc);

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


	// Cleanup
	free(basicVertSrc);
	free(basicFragSrc);
	free(postVertSrc);
}

void cleanupShaders() {
	glDeleteProgram(debugShader);

	glDeleteProgram(textShader);
	glDeleteProgram(snoiseShader);
	glDeleteProgram(rnoiseShader);

	glDeleteProgram(atmosphereShader);
	glDeleteProgram(skyShader);
	glDeleteProgram(moonShader);

	glDeleteProgram(snowShader);
	glDeleteProgram(updateSnowShader);
	glDeleteProgram(iceShader);

	glDeleteProgram(characterShader);
	glDeleteProgram(shadowCharacterShader);

	glDeleteProgram(treeShader);
	glDeleteProgram(shadowTreeShader);
}