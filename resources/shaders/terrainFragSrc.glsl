#version 330 core

void main() {
	vec3 color = vec3(0.4, 0.8, 0.4);
	gl_FragColor = vec4(color * 0.3, 1.0);
}