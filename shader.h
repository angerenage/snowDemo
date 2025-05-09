#pragma once

#include <glad/glad.h>

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
extern GLuint updateSnowShader;
extern GLuint iceShader;

extern GLuint terrainShader;
extern GLuint grassCompShader;
extern GLuint grassShader;

extern GLuint characterShader;
extern GLuint shadowCharacterShader;

extern GLuint treeShader;
extern GLuint shadowTreeShader;

extern GLuint lightShader;

void initShaders();
void cleanupShaders();