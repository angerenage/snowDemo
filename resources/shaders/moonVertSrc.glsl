#version 330 core

layout(location=0) in vec2 position;
layout(location=1) in float radius;
layout(location=2) in float gray;

out vec3 color;

void main() {
	color = vec3(gray);
	gl_PointSize = radius * 2.0;
	gl_Position = vec4(position.x * 2.0 - 1.0, 1.0 - position.y * 2.0, 0.0, 1.0);
}