#version 430 core
#define NUM_LIGHTS 11

layout(std430, binding = 0) buffer StorageBuffer {
	vec3 lightPositions[];
};

in vec3 fragPos;
in vec3 fragNormal;

out vec4 fragColor;

uniform vec3 lightPos;

vec3 calculate_point_lighting(vec3 normal, float moonIntensity) {
	vec3 color = vec3(0.0);

	for (int i = 0; i < NUM_LIGHTS; i++) {
		vec3 pointLightPos = lightPositions[i];

		vec3 lightDir = normalize(pointLightPos - fragPos);

		float diff = max(dot(normal, lightDir), 0.0);
		float distance = length(pointLightPos - fragPos);
		float fade = 1.0 - smoothstep(0.0, 8.0, distance);
		float attenuation = fade / (1.0 + 0.09 * distance);
		vec3 lightColor = vec3(1.0, 0.5, 0.0) / NUM_LIGHTS;

		color += diff * lightColor * attenuation * (1.0 - moonIntensity);
	}

	return color;
}

void main() {
	vec3 bumpedNormal = normalize(fragNormal + 0.5 * (vec3( 
		fract(sin(dot(floor(fragPos.xy * 50.0), vec2(12.9898, 78.233))) * 43758.5453), 
		fract(sin(dot(floor(fragPos.yz * 50.0), vec2(93.9898, 67.345))) * 43758.5453), 
		fract(sin(dot(floor(fragPos.zx * 50.0), vec2(56.789, 43.321))) * 43758.5453)
	) - 0.5));
	vec3 lightDir = normalize(lightPos - fragPos);

	float moonIntensity = max(dot(bumpedNormal, lightDir), 0.0);
	vec3 pointLighting = calculate_point_lighting(bumpedNormal, moonIntensity);

	fragColor = vec4(pointLighting * 0.5, 1.0);
}