#version 330 core

in float id;

out vec4 fragColor;

uniform float time;

float noise(float value) {
	return fract(sin(value * 0.1) * 1e4) * 0.5 + value / 1e2;
}

void main() {
	if (noise(id) > time * 7.0) discard;
	fragColor = vec4(1.0);
}