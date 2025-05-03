#pragma once

enum ShaderOffset {
	shader_atmosphereFragSrc = 0,
	shader_basicFragSrc = 9877,
	shader_basicVertSrc = 9913,
	shader_debugFragSrc = 9989,
	shader_snoiseFragSrc = 10034,
	shader_postVertSrc = 11825,
	shader_characterFragSrc = 11865,
	shader_moonVertSrc = 12643,
	shader_lightFragSrc = 12788,
	shader_characterVertSrc = 12972,
	shader_iceFragSrc = 13706,
	shader_lightVertSrc = 13978,
	shader_moonFragSrc = 14806,
	shader_rnoiseFragSrc = 14923,
	shader_skyFragSrc = 14993,
	shader_skyVertSrc = 15052,
	shader_snowFragSrc = 15131,
	shader_snowTCSSrc = 16586,
	shader_snowTESCSrc = 17319,
	shader_snowVertSrc = 19143,
	shader_textFragSrc = 19416,
	shader_textVertSrc = 19574,
	shader_treeFragSrc = 19675,
	shader_treeVertSrc = 20671,
	shader_updateSnowFragSrc = 20943,
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
