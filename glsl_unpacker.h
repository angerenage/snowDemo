#pragma once

enum ShaderOffset {
	shader_skyVertSrc = 0,
	shader_grassFragSrc = 73,
	shader_atmosphereFragSrc = 775,
	shader_basicFragSrc = 10356,
	shader_basicVertSrc = 10391,
	shader_debugFragSrc = 10470,
	shader_snoiseFragSrc = 10507,
	shader_postVertSrc = 12291,
	shader_characterFragSrc = 12331,
	shader_characterVertSrc = 13073,
	shader_grassCompSrc = 13805,
	shader_snowFragSrc = 14249,
	shader_grassVertSrc = 15641,
	shader_iceFragSrc = 16312,
	shader_moonVertSrc = 16572,
	shader_lightFragSrc = 16707,
	shader_lightVertSrc = 16887,
	shader_moonFragSrc = 18065,
	shader_rnoiseFragSrc = 18185,
	shader_skyFragSrc = 18249,
	shader_snowTCSSrc = 18305,
	shader_snowTESCSrc = 18993,
	shader_snowVertSrc = 20835,
	shader_terrainFragSrc = 21111,
	shader_terrainVertSrc = 21492,
	shader_textFragSrc = 21743,
	shader_textVertSrc = 21888,
	shader_transitionFragSrc = 21991,
	shader_treeFragSrc = 22015,
	shader_treeVertSrc = 22959,
	shader_updateSnowFragSrc = 23221,
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
extern const char* uniform_instanceCount;
extern const char* uniform_shadowMap;
extern const char* uniform_areaSize;
extern const char* uniform_seed;
extern const char* uniform_characterPos;
extern const char* uniform_offset;
extern const char* uniform_heightMap;
extern const char* uniform_heightmapArray;
extern const char* uniform_viewPos;
extern const char* uniform_heightTex;
extern const char* uniform_shadowProjection;
extern const char* uniform_shadowView;
extern const char* uniform_chunks;
extern const char* uniform_size;
extern const char* uniform_worldZOffset;
extern const char* uniform_aspectRatio;
extern const char* uniform_opacity;

char* getShaderSourceFromFile(const char* compressedText, size_t offset);
