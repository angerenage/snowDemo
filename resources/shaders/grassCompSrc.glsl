#version 430 core

layout(local_size_x = 64) in;

layout(std430, binding = 2) buffer GrassBuffer {
	vec4 positions[];
};

uniform uint instanceCount;
uniform float areaSize;
uniform float seed;

float hash(float n) {
	return fract(sin(n) * 43758.5453123);
}

vec2 rand2(uint i, float seed) {
	float n = float(i) + seed * 1000.0;
	return vec2(
		hash(n),
		hash(n + 1.0)
	);
}

void main() {
	uint i = gl_GlobalInvocationID.x;
	if (i >= instanceCount) return;

	vec2 r = rand2(i, seed);
	float x = r.x * areaSize - areaSize * 0.5;
	float z = r.y * areaSize - areaSize * 0.5;
	float y = 0.0;
	float scale = 1.0;

	positions[i] = vec4(x, y, z, scale);
}
