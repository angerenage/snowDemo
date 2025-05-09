#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec2 offset;

uniform sampler2D heightMap;

void main() {
	vec2 uv = (position.xz / 20.0 + offset) * 0.5 + 0.5;
	float height = texture(heightMap, uv).r * 10.0;

	vec3 displacedPos = position + vec3(offset.x, 0.0, offset.y) * 20.0;
	displacedPos.y += height;

	gl_Position = projection * view * model * vec4(displacedPos, 1.0);
}