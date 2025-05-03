#pragma once

enum ShaderOffset {
	shader_atmosphereFragSrc = 304,
	shader_basicFragSrc = 6912,
	shader_basicVertSrc = 6940,
	shader_debugFragSrc = 6972,
	shader_snoiseFragSrc = 6997,
	shader_postVertSrc = 8227,
	shader_characterFragSrc = 8246,
	shader_moonVertSrc = 8370,
	shader_lightFragSrc = 8434,
	shader_characterVertSrc = 8514,
	shader_iceFragSrc = 8820,
	shader_lightVertSrc = 8927,
	shader_moonFragSrc = 9246,
	shader_rnoiseFragSrc = 9296,
	shader_skyFragSrc = 9314,
	shader_skyVertSrc = 9354,
	shader_snowFragSrc = 9396,
	shader_snowTCSSrc = 9792,
	shader_snowTESCSrc = 9995,
	shader_snowVertSrc = 10945,
	shader_textFragSrc = 11085,
	shader_textVertSrc = 11166,
	shader_treeFragSrc = 11224,
	shader_treeVertSrc = 11463,
	shader_updateSnowFragSrc = 11559,
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
extern const char* uniform_heightmapArray;
extern const char* uniform_characterPos;
extern const char* uniform_offset;
extern const char* uniform_viewPos;
extern const char* uniform_shadowMap;
extern const char* uniform_heightTex;
extern const char* uniform_shadowProjection;
extern const char* uniform_shadowView;
extern const char* uniform_chunks;
extern const char* uniform_size;
extern const char* uniform_worldZOffset;
extern const char* uniform_aspectRatio;

char* getShaderSourceFromFile(const char* compressedText, size_t offset);
