#version 330 core

in vec3 color;

out vec4 fragColor;

void main() {
	vec2 uv = gl_PointCoord - 0.5;

	float dist = length(uv);
	if (dist > 0.5) discard;

	fragColor = vec4(pow(color, vec3(1.5)), 1.0);
}