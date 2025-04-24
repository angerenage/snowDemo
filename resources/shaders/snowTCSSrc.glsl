#version 430 core

layout(vertices=3) out;

in vec2 uv[];
in vec2 texCoords[];
in vec3 posWorld[];
in flat int instanceIndex[];

out vec2 tcsUV[];
out vec2 tcsTexCoords[];
out vec3 tcsPosWorld[];
out flat int tcsInstanceIndex[];

uniform vec2 characterPos;

void main() {
	tcsUV[gl_InvocationID] = uv[gl_InvocationID];
	tcsTexCoords[gl_InvocationID] = texCoords[gl_InvocationID];
	tcsPosWorld[gl_InvocationID] = posWorld[gl_InvocationID];
	tcsInstanceIndex[gl_InvocationID] = instanceIndex[gl_InvocationID];

	float distance = length(characterPos - tcsPosWorld[gl_InvocationID].xz);
	float tessellation = mix(2.0, 10.0, smoothstep(5.0, 1.0, distance));
	tessellation = clamp(tessellation, 2.0, 10.0);

	gl_TessLevelInner[0] = tessellation;
	gl_TessLevelOuter[0] = 2.0;
	gl_TessLevelOuter[1] = 2.0;
	gl_TessLevelOuter[2] = 2.0;
}