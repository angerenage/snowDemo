#pragma once

enum ShaderOffset {
	shader_atmosphereFragSrc = 0,
	shader_basicFragSrc = 8630,
	shader_basicVertSrc = 8668,
	shader_debugFragSrc = 8710,
	shader_snoiseFragSrc = 8745,
	shader_postVertSrc = 10644,
	shader_characterFragSrc = 10689,
	shader_debugVertSrc = 11506,
	shader_characterVertSrc = 11584,
	shader_iceFragSrc = 12250,
	shader_iceVertSrc = 12281,
	shader_rnoiseFragSrc = 12434,
	shader_skyFragSrc = 12500,
	shader_skyVertSrc = 12556,
	shader_snowFragSrc = 12634,
	shader_snowTCSSrc = 14135,
	shader_snowTESCSrc = 14758,
	shader_snowVertSrc = 15936,
	shader_textFragSrc = 16219,
	shader_textVertSrc = 16377,
	shader_treeFragSrc = 16480,
	shader_treeVertSrc = 17515,
	shader_updateSnowFragSrc = 17768,
};

extern const char* uniform_projection;
extern const char* uniform_view;
extern const char* uniform_time;
extern const char* uniform_resolution;
extern const char* uniform_lightPos;
extern const char* uniform_sunPosition;
extern const char* uniform_model;
extern const char* uniform_noiseTex;
extern const char* uniform_bones;
extern const char* uniform_tex;
extern const char* uniform_shadowProjection;
extern const char* uniform_reflection;
extern const char* uniform_shadowMap;
extern const char* uniform_reflectionView;
extern const char* uniform_skybox;
extern const char* uniform_characterPos;
extern const char* uniform_offset;
extern const char* uniform_viewPos;
extern const char* uniform_heightmapArray;
extern const char* uniform_heightTex;
extern const char* uniform_shadowView;
extern const char* uniform_size;
extern const char* uniform_aspectRatio;
extern const char* uniform_previousDepthMap;

char* getShaderSourceFromFile(const char* compressedText, size_t offset);
