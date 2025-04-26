#pragma once

enum ShaderOffset {
	shader_treeVertSrc = 182,
	shader_treeFragSrc = 268,
	shader_textVertSrc = 460,
	shader_snowVertSrc = 516,
	shader_snowTESCSrc = 645,
	shader_snowFragSrc = 1121,
	shader_snoiseFragSrc = 1527,
	shader_snowTCSSrc = 2763,
	shader_atmosphereFragSrc = 2982,
	shader_updateSnowFragSrc = 8448,
	shader_debugVertSrc = 8498,
	shader_characterFragSrc = 8523,
	shader_characterVertSrc = 8663,
	shader_skyVertSrc = 8955,
	shader_iceFragSrc = 8993,
	shader_iceVertSrc = 9013,
	shader_rnoiseFragSrc = 9072,
	shader_debugFragSrc = 9096,
	shader_textFragSrc = 9118,
	shader_basicFragSrc = 9213,
	shader_skyFragSrc = 9234,
	shader_basicVertSrc = 9268,
	shader_postVertSrc = 9298,
};

extern const char* uniform_size;
extern const char* uniform_shadowView;
extern const char* uniform_shadowProjection;
extern const char* uniform_previousDepthMap;
extern const char* uniform_heightTex;
extern const char* uniform_shadowMap;
extern const char* uniform_viewPos;
extern const char* uniform_view;
extern const char* uniform_reflection;
extern const char* uniform_heightmapArray;
extern const char* uniform_time;
extern const char* uniform_resolution;
extern const char* uniform_characterPos;
extern const char* uniform_tex;
extern const char* uniform_skybox;
extern const char* uniform_model;
extern const char* uniform_aspectRatio;
extern const char* uniform_sunPosition;
extern const char* uniform_offset;
extern const char* uniform_projection;
extern const char* uniform_noiseTex;
extern const char* uniform_reflectionView;
extern const char* uniform_lightPos;

char* getShaderSourceFromFile(const char* compressedText, size_t offset);
