#include "cameraController.h"

vec2 screenSize = {800.0, 600.0};

static float cameraYaw = 0.0f;
static float cameraPitch = 0.0f;
float lastMouseX = 0.0f;
float lastMouseY = 0.0f;
bool firstMouse = true;

mat4 cameraView = {0};
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
	cameraView = viewMatrix(cameraPos, (vec3){0.0, 0.0, 0.0}, (vec3){0.0f, 1.0f, 0.0f});
}

void defaultCameraTransforms(vec3 *pos, vec3 *dir, float distance, vec2 angles) {
	float yaw = angles.x;
	float pitch = angles.y;
	float x = cosf(yaw) * cosf(pitch);
	float y = sinf(pitch);
	float z = sinf(yaw) * cosf(pitch);

	vec3 direction = (vec3){x, y, z};
	vec3 position = vec3_scale(direction, -distance);
	
	*pos = position;
	*dir = direction;
}

mat4 reflectionCameraMatrix(vec3 *reflectionDirection, const vec3 *normal, float distance) {
	float dist = vec3_dot(*normal, cameraPos) - distance;
	vec3 reflectionPos = vec3_sub(cameraPos, vec3_scale(*normal, 2.0f * dist));

	float dotProd = vec3_dot(cameraDirection, *normal);
	*reflectionDirection = vec3_sub(cameraDirection, vec3_scale(*normal, 2.0f * dotProd));

	vec3 up = vec3_scale(vec3_sub((vec3){0.0f, 1.0f, 0.0f}, vec3_scale(*normal, 2.0f * vec3_dot((vec3){0.0f, 1.0f, 0.0f}, *normal))), -1.0f);

	return viewMatrix(reflectionPos, *reflectionDirection, up);
}