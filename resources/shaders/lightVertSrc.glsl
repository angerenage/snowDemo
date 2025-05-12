#version 430 core
#define NUM_BONES 11

layout(location=0) in vec3 origin;
layout(location=1) in vec3 direction;
layout(location=2) in vec2 interval;

layout(std430, binding = 0) buffer StorageBuffer {
	vec3 lightPositions[][2];
};

uniform mat4 projection;
uniform mat4 view;
uniform vec2 resolution;
uniform float time;

void main() {
	uint lightID = NUM_BONES + gl_VertexID;

	float t = time - interval.x;
	float duration = interval.y;
	vec3 pos = origin + direction * t + 0.5 * vec3(0.0, -4.9, 0.0) * t * t;

	if (time < interval.x || pos.y < 0.35 || t > duration + 0.2) {
		lightPositions[lightID][0] = vec3(0.0);
		lightPositions[lightID][1] = vec3(0.0);
		gl_Position = vec4(vec3(0.0), 1.0);
		gl_PointSize = 0.0;
		return;
	}

	float glow = 1.0;

	if (t < duration) {
		float normalized = clamp((t - duration * 0.9) / (duration * 0.1), 0.0, 1.0);
		glow = 1.0 + 12.0 * exp(6.0 * (normalized - 1.0));
	}
	else {
		float falloff = clamp((t - duration) / 0.2, 0.0, 1.0);
		glow = 13.0 * exp(-4.0 * falloff);
	}

	float scale = mix(1.0, 2.5, clamp(glow / 13.0, 0.0, 1.0));

	lightPositions[lightID][0] = pos;
	lightPositions[lightID][1] = vec3(0.0, 0.2, 0.4) * glow;

	vec4 viewPos = view * vec4(pos, 1.0);
	vec4 clipPos = projection * viewPos;

	vec4 offsetViewPos = view * vec4(pos + vec3(0.0, 0.5, 0.0), 1.0);
	vec4 offsetClipPos = projection * offsetViewPos;

	vec2 ndcPos = clipPos.xy / clipPos.w;
	vec2 ndcOffset = offsetClipPos.xy / offsetClipPos.w;
	float ndcSize = length(ndcOffset - ndcPos);

	gl_Position = clipPos;
	gl_PointSize = ndcSize * 0.5 * resolution.y * scale;
}