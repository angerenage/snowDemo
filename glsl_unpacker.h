#pragma once

#include <stddef.h>

enum ShaderOffset {
	shader_updateSnowFragSrc = 0,
	shader_moonFragSrc = 61,
	shader_grassFragSrc = 140,
	shader_iceFragSrc = 485,
	shader_grassCompSrc = 624,
	shader_grassVertSrc = 884,
	shader_characterVertSrc = 1222,
	shader_textVertSrc = 1576,
	shader_basicVertSrc = 1655,
	shader_moonVertSrc = 1710,
	shader_basicFragSrc = 1821,
	shader_atmosphereFragSrc = 1835,
	shader_snowTESCSrc = 4704,
	shader_postVertSrc = 5628,
	shader_rnoiseFragSrc = 5663,
	shader_skyVertSrc = 5727,
	shader_skyFragSrc = 5788,
	shader_snoiseFragSrc = 5838,
	shader_lightFragSrc = 6620,
	shader_snowFragSrc = 6724,
	shader_snowVertSrc = 7172,
	shader_transitionFragSrc = 7372,
	shader_terrainVertSrc = 7399,
	shader_snowTCSSrc = 7530,
	shader_lightVertSrc = 7981,
	shader_characterFragSrc = 8485,
	shader_treeFragSrc = 8800,
	shader_terrainFragSrc = 9246,
	shader_textFragSrc = 9464,
	shader_treeVertSrc = 9571,
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
