#version 430 core

layout(triangles, equal_spacing, ccw) in;

in vec2 tcsUV[];
in vec2 tcsTexCoords[];
in vec3 tcsPosWorld[];
in flat int tcsInstanceIndex[];

out vec2 tesUV;
out vec3 fragPos;
out vec3 fragNormal;
out vec4 shadowSpacePos;
out float footDepth;

uniform sampler2DArray heightmapArray;
uniform sampler2D heightTex;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 shadowProjection;
uniform mat4 shadowView;

uniform uvec2 chunks;

const float heightScale = 2.0;
const float heightOffset = 0.5;

float smoothMin(float a, float b, float k) {
	float h = max(k - abs(a - b), 0.0) / k;
	return min(a, b) - h * h * h * k * (1.0 / 6.0);
}

void main() {
	vec2 uv = gl_TessCoord.x * tcsUV[0] + gl_TessCoord.y * tcsUV[1] + gl_TessCoord.z * tcsUV[2];
	vec2 texCoords = gl_TessCoord.x * tcsTexCoords[0] + gl_TessCoord.y * tcsTexCoords[1] + gl_TessCoord.z * tcsTexCoords[2];
	vec3 pos = gl_TessCoord.x * tcsPosWorld[0] + gl_TessCoord.y * tcsPosWorld[1] + gl_TessCoord.z * tcsPosWorld[2];
	uint chunkIndex = tcsInstanceIndex[0];

	tesUV = texCoords;

	vec2 chunkCoord = vec2(float(chunkIndex % chunks.x), float(chunkIndex / chunks.x));
	vec2 chunkCoordOpposed = vec2(chunkCoord.x, float(chunks.y - 1) - chunkCoord.y);
	uint chunkIndexOpposed = uint(chunkCoordOpposed.x + chunkCoordOpposed.y * chunks.x);

	vec3 noiseCenter = texture(heightmapArray, vec3(uv, chunkIndex)).xyz * heightScale;
	vec3 noiseOpposed = texture(heightmapArray, vec3(uv.x, 1.0 - uv.y, chunkIndexOpposed)).xyz * heightScale;

	vec2 gradient = (chunkCoord + uv) / vec2(chunks - 1);
	pos.y += 0.01 * smoothstep(0.8, 1.0, gradient.y);

	vec3 noise = mix(noiseCenter, noiseOpposed, gradient.y);
	noise.x += heightOffset;
	vec3 perturbation = vec3(-noise.y, 1.0, -noise.z);
	fragNormal = normalize(perturbation);

	float k = 0.3;
	float height = texture(heightTex, texCoords).x;
	float blendedHeight = smoothMin(height, noise.x, k);

	footDepth = smoothstep(0.0, k, height - blendedHeight);
	footDepth = clamp((noise.x - blendedHeight) * 5.0, 0.0, 1.0);

	pos.y += blendedHeight;
	fragPos = pos;

	vec4 worldPos = vec4(pos, 1.0);
	shadowSpacePos = shadowProjection * shadowView * worldPos;
	gl_Position = projection * view * worldPos;
}