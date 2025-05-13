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

#define SCENE1_TIME 55.0f
#define SCENE2_TIME 30.0f

#define TRANSITION_TIME 3.0f

static const float patchSize = (float)(CHUNK_NBR_Z * CHUNK_SIZE);

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

	projection = projectionMatrix((float)M_PI / 4.0f, screenSize.x / screenSize.y, 0.001f, 1000.0f);
	updateCamera();

	double start = getTime();

#ifdef DEBUG
	bool first = true;
#endif

	while (running) {
		handleEvents();

		double now = getTime();
		const float ftime = (float)(now - start);

		if (currentSceneId < 0) {
			// waiting screen

			swapBuffers();
			continue;
		}

		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		updateLight(ftime, currentSceneId != 0);
		updateCharacter(ftime);

		if (currentSceneId == 0) {
			clearShadow();
			updateCamera();

			vec3 reflectionDirection;
			mat4 reflectionView = updateSnow(&reflectionDirection, &characterModel);

			updateSky(ftime * 2.0f, false, &reflectionDirection);

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
			int chunk = (int)(characterPosition.z / CHUNK_SIZE);
			if (chunk > 5 && chunk < 13) {
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

			if (ftime > SCENE1_TIME - TRANSITION_TIME) {
				glEnable(GL_BLEND);

				glUseProgram(transitionShader);

				float opacity = (ftime - (SCENE1_TIME - TRANSITION_TIME)) / TRANSITION_TIME;
				glUniform1f(glGetUniformLocation(transitionShader, uniform_opacity), opacity);

				renderScreenQuad();

				glDisable(GL_BLEND);

				if (ftime > SCENE1_TIME) {
					currentSceneId++;
					characterPosition.z = 0.0f;
					currentZOffset = 0.0f;

					characterModel = transformMatrix((vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, -(float)M_PI / 2.0f, 0.0f}, (vec3){1.0f, 1.0f, 1.0f});

					loadAnimation(&res_idle_anim);

					start = getTime();

					glDisable(GL_STENCIL_TEST);

					updateCamera();
				}
			}
		}
		else if (currentSceneId == 1) {
#ifdef DEBUG
			if (first) {
				characterPosition.z = 0.0f;
				currentZOffset = 0.0f;

				characterModel = transformMatrix((vec3){-5.0f, -0.1f, 0.9f}, (vec3){0.0f, (float)M_PI / 6.0f, 0.0f}, (vec3){1.0f, 1.0f, 1.0f});

				loadAnimation(&res_idle_anim);

				start = getTime();

				glDisable(GL_STENCIL_TEST);

				updateCamera();


				first = false;
			}
#endif

			updateSky(ftime, true, NULL);

			renderCharacter(characterShader, &projection, &cameraView, &characterModel);
			renderGrass(ftime);
			renderSky(&cameraView);

			if (ftime < TRANSITION_TIME) {
				glEnable(GL_BLEND);

				glUseProgram(transitionShader);

				float opacity = -(ftime - TRANSITION_TIME) / TRANSITION_TIME;
				glUniform1f(glGetUniformLocation(transitionShader, uniform_opacity), opacity);

				renderScreenQuad();

				glDisable(GL_BLEND);
			}

			if (ftime > SCENE2_TIME) currentSceneId++;
		}
		else {
			glClear(GL_COLOR_BUFFER_BIT);
			// Credit screen
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