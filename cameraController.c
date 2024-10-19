#include "cameraController.h"

float cameraYaw = 0.0f;
float cameraPitch = 0.0f;
float lastMouseX = 0.0f;
float lastMouseY = 0.0f;
bool firstMouse = true;

mat4 getViewMatrix() {
	vec3 cameraPos, cameraDirection;
	defaultCameraTransforms(&cameraPos, &cameraDirection, 10.0f, (vec2){cameraYaw, cameraPitch});
	return viewMatrix(cameraPos, (vec3){0.0, 5.0, 0.0}, (vec3){0.0f, 1.0f, 0.0f});
}

void defaultCameraTransforms(vec3 *pos, vec3 *dir, float distance, vec2 angles) {
	float yaw = angles.x;
	float pitch = angles.y;
	float x = cos(yaw) * cos(pitch);
	float y = sin(pitch);
	float z = sin(yaw) * cos(pitch);

	vec3 direction = (vec3){x, y, z};
	vec3 position = vec3_scale(direction, -distance);
	
	*pos = position;
	*dir = direction;
}