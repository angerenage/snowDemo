#version 430 core
#define M_PI 3.1415926535897932384626433832795
#define NUM_LIGHTS 11

layout(std430, binding = 0) buffer StorageBuffer {
	vec3 lightPositions[];
};

in vec2 tesUV;
in vec3 fragPos;
in vec3 fragNormal;
in vec4 shadowSpacePos;
in float footDepth;

out vec4 fragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D shadowMap;

float shadowCalculation() {
	vec3 projCoords = shadowSpacePos.xyz / shadowSpacePos.w;
	projCoords = projCoords * 0.5 + 0.5;
	if (projCoords.z >= 1.0)
		return 1.0;

	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;

	float bias = 0.0;//max(0.001 * (1.0 - dot(fragNormal, normalize(-lightPos))), 0.0001);
	return currentDepth - bias > closestDepth ? 0.0 : 1.0;
}

vec3 calculate_point_lighting(float moonIntensity) {
	vec3 color = vec3(0.0);

	for (int i = 0; i < NUM_LIGHTS; i++) {
		vec3 pointLightPos = lightPositions[i];

		vec3 lightDir = normalize(pointLightPos - fragPos);

		float diff = max(dot(fragNormal, lightDir), 0.0);
		float attenuation = 1.0 / (1.0 + 0.09 * length(pointLightPos - fragPos));
		vec3 lightColor = vec3(1.0, 0.5, 0.0) / NUM_LIGHTS;

		color += diff * lightColor * attenuation * (1.0 - moonIntensity);
	}

	return color;
}

void main() {
	vec3 lightDir = normalize(lightPos);
	float moonIntensity = max(dot(lightDir, vec3(0.0, 1.0, 0.0)), 0.0);
	
	float diff = max(dot(fragNormal, lightDir), 0.0);

	float shadow = shadowCalculation();
	
	vec3 ambient = vec3(0.1) * shadow * clamp(1.0 - fragPos.x / 10.0, 0.0, 1.0);
	vec3 pointLighting = calculate_point_lighting(moonIntensity * 0.5);

	vec3 viewDir = normalize(viewPos - fragPos);
	float fresnel = pow(1.0 - max(dot(viewDir, fragNormal), 0.0), 5.0);
	vec3 fresnelReflection = vec3(0.3, 0.5, 0.7) * fresnel * moonIntensity * 0.3;

	vec3 finalColor = ambient + pointLighting + fresnelReflection;
	finalColor *= 1.0 - footDepth;

	fragColor = vec4(finalColor, 1.0);
}