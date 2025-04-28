#pragma once

enum ShaderOffset {
	shader_atmosphereFragSrc = 0,
	shader_basicFragSrc = 9915,
	shader_basicVertSrc = 9950,
	shader_debugFragSrc = 9990,
	shader_snoiseFragSrc = 10026,
	shader_postVertSrc = 11919,
	shader_characterFragSrc = 11962,
	shader_debugVertSrc = 12764,
	shader_characterVertSrc = 12837,
	shader_iceFragSrc = 13554,
	shader_iceVertSrc = 13587,
	shader_moonFragSrc = 13747,
	shader_moonVertSrc = 13865,
	shader_rnoiseFragSrc = 14008,
	shader_skyFragSrc = 14074,
	shader_skyVertSrc = 14133,
	shader_snowFragSrc = 14214,
	shader_snowTCSSrc = 15686,
	shader_snowTESCSrc = 16391,
	shader_snowVertSrc = 18050,
	shader_textFragSrc = 18331,
	shader_textVertSrc = 18473,
	shader_treeFragSrc = 18575,
	shader_treeVertSrc = 19590,
	shader_updateSnowFragSrc = 19861,
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
extern const char* uniform_chunks;
extern const char* uniform_size;
extern const char* uniform_worldZOffset;
extern const char* uniform_aspectRatio;

char* getShaderSourceFromFile(const char* compressedText, size_t offset);
