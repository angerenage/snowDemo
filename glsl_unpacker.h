#pragma once

enum ShaderOffset {
	shader_skyVertSrc = 0,
	shader_grassFragSrc = 82,
	shader_atmosphereFragSrc = 459,
	shader_basicFragSrc = 10113,
	shader_basicVertSrc = 10149,
	shader_debugFragSrc = 10227,
	shader_snoiseFragSrc = 10272,
	shader_postVertSrc = 12039,
	shader_characterFragSrc = 12079,
	shader_characterVertSrc = 12871,
	shader_grassCompSrc = 13650,
	shader_snowFragSrc = 14103,
	shader_grassVertSrc = 15540,
	shader_iceFragSrc = 16176,
	shader_moonVertSrc = 16442,
	shader_lightFragSrc = 16600,
	shader_lightVertSrc = 16776,
	shader_moonFragSrc = 17628,
	shader_rnoiseFragSrc = 17743,
	shader_skyFragSrc = 17813,
	shader_snowTCSSrc = 17871,
	shader_snowTESCSrc = 18601,
	shader_snowVertSrc = 20432,
	shader_terrainFragSrc = 20704,
	shader_terrainVertSrc = 20772,
	shader_textFragSrc = 21010,
	shader_textVertSrc = 21156,
	shader_treeFragSrc = 21257,
	shader_treeVertSrc = 22245,
	shader_updateSnowFragSrc = 22522,
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
