#version 330 core

layout(location=0) in vec3 position;
layout(location=1) in vec4 instancePos;

out vec3 fragPos;
out vec2 fragUV;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec2 offset;
uniform float time;

uniform sampler2D heightMap;

const vec2 windDir = normalize(vec2(1.0, 1.0));

float noise(vec2 co) {
	return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
	fragUV = vec2(position.x, position.y / 0.508);

	vec2 uv = (instancePos.xz / 20.0 + offset) * 0.5 + 0.5;
	float height = texture(heightMap, uv).r * 10.0;

	vec2 instanceXY = instancePos.xy + offset * 2.0;
	vec2 movingWind = instanceXY + windDir * time;
	float phase = dot(movingWind, vec2(1.0)) + noise(instanceXY) * 3.14;
	float sway = sin(phase) * fragUV.y;

	vec3 animatedPos = position;
	animatedPos.xz += sway * 0.04;

	vec3 worldPos = animatedPos * instancePos.w + instancePos.xyz + vec3(offset.x, 0.0, offset.y) * 20.0;
	worldPos.y += height;

	fragPos = worldPos;
	gl_Position = projection * view * model * vec4(worldPos, 1.0);
}
