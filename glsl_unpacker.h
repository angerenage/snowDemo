#pragma once

enum ShaderOffset {
	shader_atmosphereFragSrc = 262,
	shader_basicFragSrc = 6959,
	shader_basicVertSrc = 6981,
	shader_debugFragSrc = 7006,
	shader_snoiseFragSrc = 7028,
	shader_postVertSrc = 8249,
	shader_characterFragSrc = 8267,
	shader_debugVertSrc = 8410,
	shader_characterVertSrc = 8434,
	shader_iceFragSrc = 8725,
	shader_iceVertSrc = 8744,
	shader_moonFragSrc = 8801,
	shader_moonVertSrc = 8866,
	shader_rnoiseFragSrc = 8947,
	shader_skyFragSrc = 8971,
	shader_skyVertSrc = 9002,
	shader_snowFragSrc = 9036,
	shader_snowTCSSrc = 9436,
	shader_snowTESCSrc = 9633,
	shader_snowVertSrc = 10082,
	shader_textFragSrc = 10202,
	shader_textVertSrc = 10290,
	shader_treeFragSrc = 10351,
	shader_treeVertSrc = 10557,
	shader_updateSnowFragSrc = 10644,
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
extern const char* uniform_shadowProjection;
extern const char* uniform_reflection;
extern const char* uniform_shadowMap;
extern const char* uniform_reflectionView;
extern const char* uniform_characterPos;
extern const char* uniform_offset;
extern const char* uniform_viewPos;
extern const char* uniform_heightmapArray;
extern const char* uniform_heightTex;
extern const char* uniform_shadowView;
extern const char* uniform_size;
extern const char* uniform_aspectRatio;

char* getShaderSourceFromFile(const char* compressedText, size_t offset);
