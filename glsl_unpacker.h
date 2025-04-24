#pragma once

enum ShaderOffset {
	shader_atmosphereFragSrc = 152,
	shader_basicFragSrc = 5721,
	shader_basicVertSrc = 5747,
	shader_debugFragSrc = 5771,
	shader_snoiseFragSrc = 5792,
	shader_postVertSrc = 7009,
	shader_characterFragSrc = 7026,
	shader_debugVertSrc = 7176,
	shader_characterVertSrc = 7201,
	shader_iceFragSrc = 7442,
	shader_iceVertSrc = 7461,
	shader_rnoiseFragSrc = 7516,
	shader_skyFragSrc = 7536,
	shader_skyVertSrc = 7575,
	shader_snowFragSrc = 7614,
	shader_snowTCSSrc = 8021,
	shader_snowTESCSrc = 8196,
	shader_snowVertSrc = 8617,
	shader_textFragSrc = 8704,
	shader_textVertSrc = 8794,
	shader_treeFragSrc = 8846,
	shader_treeVertSrc = 9048,
	shader_updateSnowFragSrc = 9133,
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
extern const char* uniform_heightTex;
extern const char* uniform_shadowView;
extern const char* uniform_size;
extern const char* uniform_aspectRatio;
extern const char* uniform_previousDepthMap;

char* getShaderSourceFromFile(const char* compressedText, size_t offset);
