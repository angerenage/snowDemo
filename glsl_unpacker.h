#pragma once

enum ShaderOffset {
	shader_skyVertSrc = 0,
	shader_grassFragSrc = 74,
	shader_atmosphereFragSrc = 441,
	shader_basicFragSrc = 10020,
	shader_basicVertSrc = 10056,
	shader_debugFragSrc = 10136,
	shader_snoiseFragSrc = 10173,
	shader_postVertSrc = 11948,
	shader_characterFragSrc = 11991,
	shader_characterVertSrc = 12774,
	shader_grassCompSrc = 13523,
	shader_snowFragSrc = 13968,
	shader_grassVertSrc = 15388,
	shader_iceFragSrc = 16027,
	shader_moonVertSrc = 16286,
	shader_lightFragSrc = 16423,
	shader_lightVertSrc = 16582,
	shader_moonFragSrc = 17783,
	shader_rnoiseFragSrc = 17878,
	shader_skyFragSrc = 17942,
	shader_snowTCSSrc = 18000,
	shader_snowTESCSrc = 18631,
	shader_snowVertSrc = 20434,
	shader_terrainFragSrc = 20687,
	shader_terrainVertSrc = 20747,
	shader_textFragSrc = 20999,
	shader_textVertSrc = 21136,
	shader_transitionFragSrc = 21234,
	shader_treeFragSrc = 21258,
	shader_treeVertSrc = 22241,
	shader_updateSnowFragSrc = 22504,
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
extern const char* uniform_tex;
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
