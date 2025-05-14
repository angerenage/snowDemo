#pragma once

#include <stddef.h>

enum ShaderOffset {
	shader_skyVertSrc = 94,
	shader_grassFragSrc = 123,
	shader_atmosphereFragSrc = 218,
	shader_basicFragSrc = 2061,
	shader_basicVertSrc = 2064,
	shader_snoiseFragSrc = 2087,
	shader_postVertSrc = 2586,
	shader_characterFragSrc = 2597,
	shader_characterVertSrc = 2728,
	shader_snowFragSrc = 2928,
	shader_grassVertSrc = 3125,
	shader_grassCompSrc = 3265,
	shader_iceFragSrc = 3421,
	shader_moonVertSrc = 3473,
	shader_lightFragSrc = 3527,
	shader_lightVertSrc = 3574,
	shader_moonFragSrc = 3856,
	shader_rnoiseFragSrc = 3892,
	shader_skyFragSrc = 3909,
	shader_snowTCSSrc = 3946,
	shader_snowTESCSrc = 4103,
	shader_snowVertSrc = 4597,
	shader_terrainFragSrc = 4720,
	shader_terrainVertSrc = 4760,
	shader_textFragSrc = 4787,
	shader_textVertSrc = 4845,
	shader_transitionFragSrc = 4885,
	shader_treeFragSrc = 4902,
	shader_treeVertSrc = 5063,
	shader_updateSnowFragSrc = 5145,
};

extern const char* uniform_view;
extern const char* uniform_time;
extern const char* uniform_lightPos;
extern const char* uniform_resolution;
extern const char* uniform_sunPosition;
extern const char* uniform_skybox;
extern const char* uniform_moonPosition;
extern const char* uniform_model;
extern const char* uniform_noiseTex;
extern const char* uniform_previousDepthMap;
extern const char* uniform_moonTex;
extern const char* uniform_projection;
extern const char* uniform_instanceCount;
extern const char* uniform_shadowMap;
extern const char* uniform_areaSize;
extern const char* uniform_seed;
extern const char* uniform_characterPos;
extern const char* uniform_offset;
extern const char* uniform_heightMap;
extern const char* uniform_heightmapArray;
extern const char* uniform_viewPos;
extern const char* uniform_heightTex;
extern const char* uniform_shadowProjection;
extern const char* uniform_shadowView;
extern const char* uniform_chunks;
extern const char* uniform_size;
extern const char* uniform_worldZOffset;
extern const char* uniform_aspectRatio;
extern const char* uniform_opacity;

char* getShaderSourceFromFile(const char* compressedText, size_t offset);
