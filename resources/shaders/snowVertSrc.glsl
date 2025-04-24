#version 430 core

layout(location=0) in vec3 position;

out vec2 uv;
out vec2 texCoords;
out vec3 posWorld;

uniform mat4 model;
uniform float size;
uniform vec3 offset;
uniform vec2 characterPos;

void main() {
	uv = vec2((position.xz / size + 1.0) / 2.0);

	vec4 pos = model * vec4(position, 1.0);
	posWorld = pos.xyz + offset;
	texCoords = ((posWorld.xz - characterPos) / size + 1.0) / 2.0;

	gl_Position = pos;
}