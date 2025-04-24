#version 330 core

in vec2 fragPos;

uniform sampler2D previousDepthMap;
uniform vec2 offset;

void main() {
	vec2 uv = (fragPos + 1.0) * 0.5 + offset;
	gl_FragDepth = texture(previousDepthMap, uv).r;
}