#version 330 core

in vec2 fragPos;
out vec4 fragColor;

void main() {
	vec3 baseColor = vec3(0.2, 0.5, 0.2);
	vec3 tipColor = vec3(0.4, 0.8, 0.4);
	vec3 aoColor = vec3(0.2);

	float height = clamp(fragPos.y, 0.0, 1.0);

	vec3 gradient = mix(baseColor, tipColor, height);

	float ao = smoothstep(0.0, 0.2, height);
	vec3 shaded = mix(aoColor, gradient, ao);

	float tipFactor = smoothstep(0.5, 1.0, height);
	vec3 tipGlow = vec3(0.0, 0.05, 0.0) * tipFactor;

	fragColor = vec4(shaded + tipGlow, 1.0);
}
