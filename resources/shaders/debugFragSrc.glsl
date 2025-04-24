#version 330 core

in vec2 fragPos;

out vec4 fragColor;

uniform sampler2D tex;

void main() {
	fragColor = vec4(texture(tex, fragPos).xyz, 1.0);
}