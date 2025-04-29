#pragma once

enum ShaderOffset {
	shader_atmosphereFragSrc = 0,
	shader_basicFragSrc = 10121,
	shader_basicVertSrc = 10155,
	shader_debugFragSrc = 10201,
	shader_snoiseFragSrc = 10240,
	shader_postVertSrc = 12082,
	shader_characterFragSrc = 12116,
	shader_debugVertSrc = 12927,
	shader_characterVertSrc = 12988,
	shader_iceFragSrc = 13704,
	shader_iceVertSrc = 13705,
	shader_moonFragSrc = 13706,
	shader_moonVertSrc = 13824,
	shader_rnoiseFragSrc = 13963,
	shader_skyFragSrc = 14029,
	shader_skyVertSrc = 14084,
	shader_snowFragSrc = 14168,
	shader_snowTCSSrc = 15672,
	shader_snowTESCSrc = 16376,
	shader_snowVertSrc = 18246,
	shader_textFragSrc = 18525,
	shader_textVertSrc = 18676,
	shader_treeFragSrc = 18776,
	shader_treeVertSrc = 19805,
	shader_updateSnowFragSrc = 20077,
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
extern const char* uniform_characterPos;
extern const char* uniform_offset;
extern const char* uniform_viewPos;
extern const char* uniform_shadowMap;
extern const char* uniform_heightmapArray;
extern const char* uniform_heightTex;
extern const char* uniform_shadowProjection;
extern const char* uniform_shadowView;
extern const char* uniform_chunks;
extern const char* uniform_size;
extern const char* uniform_worldZOffset;
extern const char* uniform_aspectRatio;

char* getShaderSourceFromFile(const char* compressedText, size_t offset);
