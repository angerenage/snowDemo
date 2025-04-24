#pragma once

enum ShaderOffset {
	shader_atmosphereFragSrc = 193,
	shader_basicFragSrc = 5880,
	shader_basicVertSrc = 5906,
	shader_debugFragSrc = 5935,
	shader_snoiseFragSrc = 5954,
	shader_postVertSrc = 7192,
	shader_characterFragSrc = 7210,
	shader_debugVertSrc = 7353,
	shader_characterVertSrc = 7375,
	shader_iceFragSrc = 7660,
	shader_iceVertSrc = 7677,
	shader_rnoiseFragSrc = 7731,
	shader_skyFragSrc = 7756,
	shader_skyVertSrc = 7789,
	shader_snowFragSrc = 7823,
	shader_snowTCSSrc = 8230,
	shader_snowTESCSrc = 8430,
	shader_snowVertSrc = 8864,
	shader_textFragSrc = 8989,
	shader_textVertSrc = 9082,
	shader_treeFragSrc = 9131,
	shader_treeVertSrc = 9330,
	shader_updateSnowFragSrc = 9413,
};

extern const char* uniform_projection;
extern const char* uniform_view;
extern const char* uniform_time;
extern const char* uniform_resolution;
extern const char* uniform_lightPos;
extern const char* uniform_sunPosition;
extern const char* uniform_model;
extern const char* uniform_noiseTex;
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
