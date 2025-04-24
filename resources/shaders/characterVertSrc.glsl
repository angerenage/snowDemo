#version 430 core

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in uint material;
layout(location=3) in uint bone;

layout(std430, binding = 0) buffer StorageBuffer {
	vec3 lightPositions[];
};

struct Bone {
	vec3 position;
	vec3 lightPosition;
	mat3 rotation;
	uint parent;
};
layout(std430, binding = 1) buffer BoneBuffer {
	Bone bones[];
};

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 fragPos;
out vec3 fragNormal;
flat out uint fragMaterial;

void main() {
	uint boneID = bone;
	fragPos = bones[0].position;

	while (boneID != bones[boneID].parent) {
		fragPos += bones[bones[boneID].parent].rotation * bones[boneID].position;
		boneID = bones[boneID].parent;
	}

	mat3 rotation = bones[bone].rotation;
	lightPositions[bone] = vec3(model * vec4(fragPos + rotation * bones[bone].lightPosition, 1.0));
	fragPos += rotation * position;

	fragMaterial = material;
	fragNormal = normalize(transpose(inverse(mat3(model))) * rotation * normal);
	gl_Position = projection * view * model * vec4(fragPos, 1.0);
}