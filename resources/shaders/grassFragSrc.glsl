#version 330 core
#define NUM_LIGHTS 11

in vec3 fragPos;
in vec2 fragUV;
out vec4 fragColor;

uniform vec3 lightPos;

layout(std430, binding = 0) buffer StorageBuffer {
	vec3 lightPositions[][2];
};

vec3 calculate_point_lighting(float lightIntensity) {
	vec3 color = vec3(0.0);

	for (int i = 0; i < NUM_LIGHTS; i++) {
		vec3 pointLightPos = lightPositions[i][0];

		float attenuation = 1.0 / (1.0 + 0.09 * length(pointLightPos - fragPos));
		vec3 lightColor = lightPositions[i][1];

		color += lightColor * attenuation * (1.0 - lightIntensity);
	}

	return color;
}

void main() {
	vec3 baseColor = vec3(0.2, 0.5, 0.2);
	vec3 tipColor = vec3(0.4, 0.8, 0.4);
	vec3 aoColor = vec3(0.2);

	float height = clamp(fragUV.y, 0.0, 1.0);

	vec3 gradient = mix(baseColor, tipColor, height);

	float ao = smoothstep(0.0, 0.2, height);
	vec3 shaded = mix(aoColor, gradient, ao);

	float tipFactor = smoothstep(0.5, 1.0, height);
	vec3 tipGlow = vec3(0.0, 0.05, 0.0) * tipFactor;

	float rawSunDot = dot(vec3(0.0, 1.0, 0.0), normalize(lightPos));
	float dayFactor = smoothstep(0.0, 0.3, rawSunDot);
	float lightIntensity = mix(0.2, 1.0, dayFactor);

	shaded *= lightIntensity;

	vec3 pointLighting = calculate_point_lighting(0.7);

	shaded += pointLighting;
	fragColor = vec4(shaded + tipGlow, 1.0);
}
