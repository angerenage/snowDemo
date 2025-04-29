#pragma once

#include <glad/glad.h>
#include <stdlib.h>
#include <stdio.h>

#include "resources.h"
#include "glsl_unpacker.h"

extern GLuint debugShader;
extern GLuint basicShader;

extern GLuint textShader;
extern GLuint snoiseShader;
extern GLuint rnoiseShader;

extern GLuint atmosphereShader;
extern GLuint skyShader;
extern GLuint moonShader;

extern GLuint snowShader;
extern GLuint shadowSnowShader;
extern GLuint updateSnowShader;

extern GLuint characterShader;
extern GLuint shadowCharacterShader;

extern GLuint treeShader;
extern GLuint shadowTreeShader;

void initShaders();
void cleanupShaders();