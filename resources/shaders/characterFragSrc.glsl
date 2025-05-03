#version 430 core
#define NUM_LIGHTS 21

layout(std430, binding = 0) buffer StorageBuffer {
	vec3 lightPositions[][2];
};

in vec3 fragPos;
in vec3 fragNormal;
flat in uint fragMaterial;

out vec4 fragColor;

uniform vec3 lightPos;

vec3 calculate_point_lighting(float lightIntensity) {
	vec3 color = vec3(0.0);

	for (int i = 0; i < NUM_LIGHTS; i++) {
		vec3 pointLightPos = lightPositions[i][0];

		vec3 lightDir = normalize(pointLightPos - fragPos);

		float diff = max(dot(fragNormal, lightDir), 0.0);

		float attenuation = 1.0 / (1.0 + 0.09 * length(pointLightPos - fragPos));
		vec3 lightColor = lightPositions[i][1];

		color += diff * lightColor * attenuation * (1.0 - lightIntensity);
	}

	return color;
}

void main() {
	if (fragMaterial == 1) {
		fragColor = vec4(1.0, 1.0, 1.0, 1.0);
		return;
	}

	vec3 baseAmbient = vec3(0.05);

	float lightIntensity = max(dot(normalize(lightPos), vec3(0.0, 1.0, 0.0)), 0.0);
	vec3 ambient = baseAmbient + min(vec3(0.3, 0.3, 0.4) * lightIntensity, 1.0) * 0.05;

	vec3 pointLighting = calculate_point_lighting(lightIntensity);

	fragColor = vec4(ambient + pointLighting, 1.0);
}