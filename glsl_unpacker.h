#pragma once

enum ShaderOffset {
	shader_skyVertSrc = 0,
	shader_grassFragSrc = 71,
	shader_atmosphereFragSrc = 961,
	shader_basicFragSrc = 10542,
	shader_basicVertSrc = 10573,
	shader_debugFragSrc = 10650,
	shader_snoiseFragSrc = 10687,
	shader_postVertSrc = 12469,
	shader_characterFragSrc = 12507,
	shader_characterVertSrc = 13243,
	shader_grassCompSrc = 13949,
	shader_snowFragSrc = 14393,
	shader_grassVertSrc = 15775,
	shader_iceFragSrc = 16444,
	shader_moonVertSrc = 16704,
	shader_lightFragSrc = 16831,
	shader_lightVertSrc = 16989,
	shader_moonFragSrc = 18157,
	shader_rnoiseFragSrc = 18251,
	shader_skyFragSrc = 18315,
	shader_snowTCSSrc = 18369,
	shader_snowTESCSrc = 18985,
	shader_snowVertSrc = 20768,
	shader_terrainFragSrc = 21022,
	shader_terrainVertSrc = 21571,
	shader_textFragSrc = 21822,
	shader_textVertSrc = 21967,
	shader_transitionFragSrc = 22068,
	shader_treeFragSrc = 22092,
	shader_treeVertSrc = 23034,
	shader_updateSnowFragSrc = 23294,
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
