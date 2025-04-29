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

void moveCamera(float xoffset, float yoffset) {
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

	updateCamera();
}

void defaultCameraTransforms(vec3 *pos, vec3 *dir, float distance, vec2 angles) {
	float yaw = angles.x;
	float pitch = angles.y;
	float x = cosf(yaw) * cosf(pitch);
	float y = sinf(pitch);
	float z = sinf(yaw) * cosf(pitch);

	vec3 direction = (vec3){x, y, z};
	vec3 position = vec3_add(characterPosition, vec3_scale(direction, -distance));
	
	*pos = position;
	*dir = direction;
}

void updateCamera() {
	defaultCameraTransforms(&cameraPos, &cameraDirection, 10.0f, (vec2){cameraYaw, cameraPitch});
	cameraView = viewMatrix(cameraPos, characterPosition, (vec3){0.0f, 1.0f, 0.0f});
}

mat4 reflectionCameraMatrix(vec3 *reflectionDirection, float height) {
	*reflectionDirection = vec3_normalize(vec3_sub(characterPosition, (vec3){
		cameraPos.x,
		2.0f * height - cameraPos.y,
		cameraPos.z
	}));	

	mat4 view = viewMatrix(cameraPos, characterPosition, (vec3){0.0f, 1.0f, 0.0f});

	for(int i = 0; i < 4; i++) {
		view.m[1][i] = -view.m[1][i];
	}
	view.m[3][1] += 2.0f * height;

	return view;
}