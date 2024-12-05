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

extern GLuint characterShader;
extern GLuint shadowCharacterShader;

void initShaders();

GLuint compileShader(const char *vShaderCode, const char *gShaderCode, const char *fShaderCode);
GLuint compileComputeShader(const char *shaderCode);

void cleanupShaders();