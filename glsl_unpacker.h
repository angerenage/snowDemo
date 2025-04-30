#pragma once

enum ShaderOffset {
	shader_atmosphereFragSrc = 0,
	shader_basicFragSrc = 10187,
	shader_basicVertSrc = 10225,
	shader_debugFragSrc = 10305,
	shader_snoiseFragSrc = 10344,
	shader_postVertSrc = 12199,
	shader_characterFragSrc = 12243,
	shader_debugVertSrc = 13054,
	shader_characterVertSrc = 13055,
	shader_iceFragSrc = 13762,
	shader_moonFragSrc = 14042,
	shader_moonVertSrc = 14163,
	shader_rnoiseFragSrc = 14310,
	shader_skyFragSrc = 14376,
	shader_skyVertSrc = 14436,
	shader_snowFragSrc = 14519,
	shader_snowTCSSrc = 16028,
	shader_snowTESCSrc = 16785,
	shader_snowVertSrc = 18669,
	shader_textFragSrc = 18957,
	shader_textVertSrc = 19117,
	shader_treeFragSrc = 19224,
	shader_treeVertSrc = 20254,
	shader_updateSnowFragSrc = 20541,
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
