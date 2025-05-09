#pragma once

enum ShaderOffset {
	shader_skyVertSrc = 0,
	shader_grassFragSrc = 82,
	shader_atmosphereFragSrc = 459,
	shader_basicFragSrc = 10111,
	shader_basicVertSrc = 10147,
	shader_debugFragSrc = 10225,
	shader_snoiseFragSrc = 10270,
	shader_postVertSrc = 12037,
	shader_characterFragSrc = 12077,
	shader_characterVertSrc = 12869,
	shader_grassCompSrc = 13648,
	shader_snowFragSrc = 14101,
	shader_grassVertSrc = 15538,
	shader_iceFragSrc = 16174,
	shader_moonVertSrc = 16440,
	shader_lightFragSrc = 16598,
	shader_lightVertSrc = 16774,
	shader_moonFragSrc = 17626,
	shader_rnoiseFragSrc = 17741,
	shader_skyFragSrc = 17811,
	shader_snowTCSSrc = 17869,
	shader_snowTESCSrc = 18599,
	shader_snowVertSrc = 20430,
	shader_terrainFragSrc = 20702,
	shader_terrainVertSrc = 20770,
	shader_textFragSrc = 21008,
	shader_textVertSrc = 21154,
	shader_treeFragSrc = 21255,
	shader_treeVertSrc = 22243,
	shader_updateSnowFragSrc = 22520,
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

char* getShaderSourceFromFile(const char* compressedText, size_t offset);
