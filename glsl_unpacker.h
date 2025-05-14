#pragma once

#include <stddef.h>

enum ShaderOffset {
	shader_updateSnowFragSrc = 55,
	shader_moonFragSrc = 88,
	shader_grassFragSrc = 120,
	shader_iceFragSrc = 209,
	shader_grassCompSrc = 267,
	shader_grassVertSrc = 428,
	shader_characterVertSrc = 570,
	shader_textVertSrc = 769,
	shader_basicVertSrc = 809,
	shader_moonVertSrc = 833,
	shader_basicFragSrc = 891,
	shader_atmosphereFragSrc = 894,
	shader_snowTESCSrc = 2752,
	shader_postVertSrc = 3248,
	shader_rnoiseFragSrc = 3261,
	shader_skyVertSrc = 3278,
	shader_skyFragSrc = 3313,
	shader_snoiseFragSrc = 3348,
	shader_lightFragSrc = 3843,
	shader_snowFragSrc = 3890,
	shader_snowVertSrc = 4084,
	shader_transitionFragSrc = 4203,
	shader_terrainVertSrc = 4220,
	shader_snowTCSSrc = 4247,
	shader_lightVertSrc = 4418,
	shader_characterFragSrc = 4707,
	shader_treeFragSrc = 4835,
	shader_terrainFragSrc = 4993,
	shader_textFragSrc = 5025,
	shader_treeVertSrc = 5078,
};

extern const char* uniform_opacity;
extern const char* uniform_size;
extern const char* uniform_shadowView;
extern const char* uniform_chunks;
extern const char* uniform_shadowProjection;
extern const char* uniform_previousDepthMap;
extern const char* uniform_heightTex;
extern const char* uniform_shadowMap;
extern const char* uniform_viewPos;
extern const char* uniform_view;
extern const char* uniform_heightmapArray;
extern const char* uniform_time;
extern const char* uniform_resolution;
extern const char* uniform_instanceCount;
extern const char* uniform_moonTex;
extern const char* uniform_heightMap;
extern const char* uniform_projection;
extern const char* uniform_skybox;
extern const char* uniform_moonPosition;
extern const char* uniform_model;
extern const char* uniform_worldZOffset;
extern const char* uniform_noiseTex;
extern const char* uniform_lightPos;
extern const char* uniform_aspectRatio;
extern const char* uniform_offset;
extern const char* uniform_sunPosition;
extern const char* uniform_areaSize;
extern const char* uniform_characterPos;
extern const char* uniform_seed;

char* getShaderSourceFromFile(const char* compressedText, size_t offset);
