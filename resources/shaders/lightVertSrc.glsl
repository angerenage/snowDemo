#version 430 core
#define NUM_BONES 11

layout(location=0) in vec3 origin;
layout(location=1) in vec3 direction;
layout(location=2) in float startTime;

layout(std430, binding = 0) buffer StorageBuffer {
	vec3 lightPositions[][2];
};

uniform mat4 projection;
uniform mat4 view;
uniform vec2 resolution;
uniform float time;

void main() {
	uint lightID = NUM_BONES + gl_VertexID;

	float t = time - startTime;
	vec3 pos = origin + direction * t + 0.5 * vec3(0.0, -4.9, 0.0) * t * t;

	if (time < startTime || pos.y < 0.35) {
		lightPositions[lightID][0] = vec3(0.0);
		lightPositions[lightID][1] = vec3(0.0);
		gl_Position = vec4(vec3(0.0), 1.0);
		gl_PointSize = 0.0;
		return;
	}

	lightPositions[lightID][0] = pos;
	lightPositions[lightID][1] = vec3(0.0, 0.2, 0.4);

	vec4 viewPos = view * vec4(pos, 1.0);
	vec4 clipPos = projection * viewPos;

	vec4 offsetViewPos = view * vec4(pos + vec3(0.0, 0.5, 0.0), 1.0);
	vec4 offsetClipPos = projection * offsetViewPos;

	vec2 ndcPos = clipPos.xy / clipPos.w;
	vec2 ndcOffset = offsetClipPos.xy / offsetClipPos.w;
	float ndcSize = length(ndcOffset - ndcPos);

	gl_Position = clipPos;
	gl_PointSize = ndcSize * 0.5 * resolution.y;
}