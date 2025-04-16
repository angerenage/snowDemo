#pragma once

#include <glad/glad.h>
#include <stdio.h>

extern GLuint debugShader;
extern GLuint basicShader;

extern GLuint textShader;
extern GLuint snoiseShader;
extern GLuint rnoiseShader;

extern GLuint atmosphereShader;
extern GLuint skyShader;

extern GLuint snowShader;
extern GLuint shadowSnowShader;
extern GLuint updateSnowShader;

extern GLuint iceShader;

extern GLuint characterShader;
extern GLuint shadowCharacterShader;

extern GLuint treeShader;
extern GLuint shadowTreeShader;
extern GLuint needleShader;

void initShaders();
void cleanupShaders();