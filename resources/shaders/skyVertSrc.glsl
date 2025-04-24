#version 330 core

layout(location=0) in vec3 position;

out vec3 fragPos;

uniform mat4 projection;
uniform mat4 view;

void main() {
	fragPos = normalize(position);
	gl_Position = projection * mat4(mat3(view)) * vec4(position, 1.0);
}