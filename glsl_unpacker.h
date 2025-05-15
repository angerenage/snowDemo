#pragma once

#include <stddef.h>

enum ShaderOffset {
	shader_skyVertSrc = 0,
	shader_grassFragSrc = 61,
	shader_atmosphereFragSrc = 389,
	shader_basicFragSrc = 3172,
	shader_basicVertSrc = 3180,
	shader_snoiseFragSrc = 3235,
	shader_postVertSrc = 4003,
	shader_characterFragSrc = 4038,
	shader_characterVertSrc = 4338,
	shader_snowFragSrc = 4689,
	shader_grassVertSrc = 5124,
	shader_grassCompSrc = 5462,
	shader_iceFragSrc = 5720,
	shader_moonVertSrc = 5861,
	shader_lightFragSrc = 5972,
	shader_lightVertSrc = 6076,
	shader_moonFragSrc = 6577,
	shader_rnoiseFragSrc = 6656,
	shader_skyFragSrc = 6722,
	shader_snowTCSSrc = 6772,
	shader_snowTESCSrc = 7223,
	shader_snowVertSrc = 8135,
	shader_terrainFragSrc = 8335,
	shader_terrainVertSrc = 8539,
	shader_textFragSrc = 8668,
	shader_textVertSrc = 8769,
	shader_transitionFragSrc = 8848,
	shader_treeFragSrc = 8875,
	shader_treeVertSrc = 9304,
	shader_updateSnowFragSrc = 9465,
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
