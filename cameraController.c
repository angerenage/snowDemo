#include "cameraController.h"

#include "window.h"
#include "character.h"

vec2 screenSize = {800.0, 600.0};

static float cameraYaw = 0.0f;
static float cameraPitch = 0.0f;
float lastMouseX = 0.0f;
float lastMouseY = 0.0f;
bool firstMouse = true;

mat4 projection = {0};
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

void updateCamera() {
	if (currentSceneId == 0) {
		cameraPos = (vec3){-6.40f, 4.75f, characterPosition.z - 6.0f};
		cameraDirection = vec3_normalize(vec3_sub(characterPosition, cameraPos));
		cameraView = viewMatrix(cameraPos, characterPosition, (vec3){0.0f, 1.0f, 0.0f});
	}
	else {
		cameraPos = (vec3){-6.5f, 1.5f, 0.0f};
		cameraDirection = (vec3){1.0f, -0.1f, 0.0f};

		vec3 target = vec3_add(cameraPos, cameraDirection);
		cameraView = viewMatrix(cameraPos, target, (vec3){0.0f, 1.0f, 0.0f});
	}
}

mat4 reflectionCameraMatrix(vec3* restrict reflectionDirection, float height) {
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