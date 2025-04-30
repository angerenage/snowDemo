#version 330 core

in vec2 fragPos;

out vec4 fragColor;

uniform sampler2DArray heightmapArray;

float noise(vec2 pos) {
    return fract(sin(dot(pos, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
	float frost = texture(heightmapArray, vec3(fragPos / 2.0, 0)).r;

	float alpha = frost * 1.5 + 0.05;
    alpha += (noise(fragPos) - 0.5) * 0.03;

	fragColor = vec4(0.85, 0.90, 1.0, clamp(alpha, 0.0, 1.0));
}