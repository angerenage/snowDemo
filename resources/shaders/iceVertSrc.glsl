#version 330 core

layout(location=0) in vec3 position;

out vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 reflectionView;
uniform mat4 projection;

void main() {
	vec4 pos = model * vec4(position, 1.0);

	vec4 reflPos = projection * reflectionView * pos;
	reflPos /= reflPos.w;

	texCoords = (reflPos.xy + 1.0) / 2.0;

	gl_Position = projection * view * pos;
}