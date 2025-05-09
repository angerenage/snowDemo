#include <stdio.h>
#include <stdbool.h>

#include "window.h"
#include "cameraController.h"
#include "shadow.h"
#include "glutils.h"
#include "shader.h"
#include "snow.h"
#include "character.h"
#include "sky.h"
#include "tree.h"
#include "grass.h"

int main() {
	initWindow(screenSize);

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_PROGRAM_POINT_SIZE);

	glDepthFunc(GL_LESS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	initShaders();
	initUtils();
	initSky();
	initShadow();
	initCharacter();
	initSnow();
	initTrees();
	initGrass();

	mat4 characterModel = rotationMatrix((vec3){0.0f, -(float)M_PI / 2.0f, 0.0f});
	loadAnimation(&res_running_anim);

	projection = projectionMatrix((float)M_PI / 4.0f, screenSize.x / screenSize.y, 0.001f, 1000.0f);
	updateCamera();

	bool skyUpdate = true;
	float start = getTime();

	bool first = true; // DEBUG

	while (running) {
		handleEvents();

		float now = getTime();
		const float ftime = now - start;

		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		updateLight(ftime, currentSceneId != 0);
		updateCharacter(ftime);

		if (currentSceneId == 0) {
			clearShadow();
			updateCamera();

			vec3 reflectionDirection;
			mat4 reflectionView = updateSnow(&reflectionDirection, &characterModel);

			if (skyUpdate) updateSky(ftime * 2.0f, false, &reflectionDirection);
			skyUpdate = !skyUpdate;

			characterModel.m[3][2] = characterPosition.z;
			int currentChunkZ = (int)((characterPosition.z - currentZOffset) / CHUNK_SIZE);

			// Shadow pass
			glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
			glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);

			renderCharacter(shadowCharacterShader, &shadowProjection, &shadowView, &characterModel);
			renderTrees(shadowTreeShader, &shadowProjection, &shadowView, &lightPosition, currentChunkZ);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, (GLsizei)screenSize.x, (GLsizei)screenSize.y);

			// Main pass
			renderCharacter(characterShader, &projection, &cameraView, &characterModel);
			renderTrees(treeShader, &projection, &cameraView, &lightPosition, currentChunkZ);
			renderSnow(&cameraView, currentChunkZ);
			renderSky(&cameraView);
			renderLights(&cameraView, ftime);

			// Ice pass
			if (characterPosition.z / CHUNK_SIZE < 13) {
				glClear(GL_DEPTH_BUFFER_BIT);

				glStencilFunc(GL_EQUAL, 1, 0xFF);
				glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
				glCullFace(GL_FRONT);

				renderCharacter(characterShader, &projection, &reflectionView, &characterModel);
				renderTrees(treeShader, &projection, &reflectionView, &lightPosition, currentChunkZ);
				renderSky(&reflectionView);
				renderLights(&reflectionView, ftime);

				glCullFace(GL_BACK);

				renderIce();

				glStencilFunc(GL_ALWAYS, 0, 0xFF);
				glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			}

			if (ftime > 55.0f) {
				currentSceneId = 1;
				start = getTime();

				glDisable(GL_STENCIL_TEST);

				updateCamera();
			}
		}
		else {
			if (first) {
				start = getTime();

				glDisable(GL_STENCIL_TEST);

				updateCamera();


				first = false;
			}

			updateSky(ftime, true, NULL);

			renderGrass(ftime);
			renderSky(&cameraView);
		}

#ifdef DEBUG
		checkOpenGLError();
#endif

		swapBuffers();
	}

	cleanupGrass();
	cleanupTrees();
	cleanupSnow();
	cleanupCharacter();
	cleanupShadow();
	cleanupSky();
	cleanupUtils();
	cleanupShaders();
	cleanupWindow();

	return 0;
}