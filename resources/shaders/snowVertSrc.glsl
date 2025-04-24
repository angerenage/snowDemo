#version 430 core

layout(location=0) in vec3 position;
layout(location=3) in vec3 instanceOffset;

out vec2 uv;
out vec2 texCoords;
out vec3 posWorld;
out flat int instanceIndex;

uniform mat4 model;
uniform float size;
uniform vec2 characterPos;

void main() {
	instanceIndex = int(instanceOffset.y);

	uv = vec2((position.xz / size + 1.0) / 2.0);

	vec4 pos = model * vec4(position, 1.0);
	posWorld = pos.xyz + vec3(instanceOffset.x, 0.0, instanceOffset.z);
	texCoords = ((posWorld.xz - characterPos) / size + 1.0) / 2.0;

	gl_Position = pos;
}