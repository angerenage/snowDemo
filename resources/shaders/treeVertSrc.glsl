#version 330 core

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout (location=3) in mat4 instanceModel;

out vec3 fragPos;
out vec3 fragNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	mat4 modelInstance = model * instanceModel;

	vec4 pos = modelInstance * vec4(position, 1.0);
	fragPos = pos.xyz;

	mat3 normalMatrix = transpose(inverse(mat3(modelInstance)));
	fragNormal = normalize(normalMatrix * normal);

	gl_Position = projection * view * pos;
}