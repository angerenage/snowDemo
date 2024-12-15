#pragma once

#include <glad/glad.h>
#include <stdio.h>

extern GLuint debugShader;

extern GLuint textShader;
extern GLuint snoiseShader;
extern GLuint rnoiseShader;

extern GLuint atmosphereShader;
extern GLuint skyShader;

extern GLuint snowShader;
extern GLuint shadowSnowShader;
extern GLuint updateSnowShader;

extern GLuint characterShader;
extern GLuint shadowCharacterShader;
extern GLuint updateCharacterShader;

void initShaders();

GLuint compileShader(const char *vShaderCode, const char *tcsShaderCode, const char *tesShaderCode, const char *gShaderCode, const char *fShaderCode);
GLuint compileComputeShader(const char *shaderCode);

void cleanupShaders();