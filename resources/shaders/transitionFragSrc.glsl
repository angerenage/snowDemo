#version 330 core

out vec4 fragColor;

uniform float opacity;

void main() {
	fragColor = vec4(vec3(0.0), opacity);
}