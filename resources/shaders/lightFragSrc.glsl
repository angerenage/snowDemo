#version 430 core

out vec4 fragColor;

void main() {
	vec2 uv = gl_PointCoord * 2.0 - 1.0;
	float r = length(uv);
	if (r > 1.0) discard;

	float alpha = pow(1.0 - r, 3.0);
	vec3 color = mix(vec3(0.0, 0.5, 1.0), vec3(1.0), 1.0 - r);

	fragColor = vec4(color, alpha);
}