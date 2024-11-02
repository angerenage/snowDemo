#include "cameraController.h"

static float cameraYaw = 0.0f;
static float cameraPitch = 0.0f;
float lastMouseX = 0.0f;
float lastMouseY = 0.0f;
bool firstMouse = true;

mat4 view = {0};
vec3 cameraPos = {0};
vec3 cameraDirection = {0};

void updateCamera(float xoffset, float yoffset) {
	if (firstMouse) {
		lastMouseX = xoffset;
		lastMouseY = yoffset;
		firstMouse = false;
	}

	float xoffsetChange = xoffset - lastMouseX;
	float yoffsetChange = lastMouseY - yoffset;

	lastMouseX = xoffset;
	lastMouseY = yoffset;

	cameraYaw += xoffsetChange * 0.005f;
	cameraPitch += yoffsetChange * 0.005f;

	if (cameraPitch > 1.57f) cameraPitch = 1.57f;
	if (cameraPitch < -1.57f) cameraPitch = -1.57f;

	defaultCameraTransforms(&cameraPos, &cameraDirection, 10.0f, (vec2){cameraYaw, cameraPitch});
	view = viewMatrix(cameraPos, (vec3){0.0, 0.0, 0.0}, (vec3){0.0f, 1.0f, 0.0f});
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