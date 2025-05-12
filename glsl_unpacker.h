#pragma once

enum ShaderOffset {
	shader_skyVertSrc = 0,
	shader_grassFragSrc = 80,
	shader_atmosphereFragSrc = 455,
	shader_basicFragSrc = 10088,
	shader_basicVertSrc = 10124,
	shader_debugFragSrc = 10200,
	shader_snoiseFragSrc = 10237,
	shader_postVertSrc = 12009,
	shader_characterFragSrc = 12049,
	shader_characterVertSrc = 12831,
	shader_grassCompSrc = 13554,
	shader_snowFragSrc = 13990,
	shader_grassVertSrc = 15392,
	shader_iceFragSrc = 16009,
	shader_moonVertSrc = 16273,
	shader_lightFragSrc = 16415,
	shader_lightVertSrc = 16591,
	shader_moonFragSrc = 17779,
	shader_rnoiseFragSrc = 17894,
	shader_skyFragSrc = 17958,
	shader_snowTCSSrc = 18008,
	shader_snowTESCSrc = 18623,
	shader_snowVertSrc = 20394,
	shader_terrainFragSrc = 20639,
	shader_terrainVertSrc = 20701,
	shader_textFragSrc = 20936,
	shader_textVertSrc = 21080,
	shader_treeFragSrc = 21181,
	shader_treeVertSrc = 22161,
	shader_updateSnowFragSrc = 22431,
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
