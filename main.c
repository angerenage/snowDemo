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

int main() {
	initWindow(screenSize);
	

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//glEnable(GL_PROGRAM_POINT_SIZE);

	glDepthFunc(GL_LESS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


	initShaders();
	initUtils();
	initSky();
	initShadow();
	initCharacter();
	initSnow();
	initTrees();

	glViewport(0, 0, (GLsizei)screenSize.x, (GLsizei)screenSize.y);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	

	mat4 characterModel = rotationMatrix((vec3){0.0f, -(float)M_PI / 2.0f, 0.0f});
	loadAnimation(&res_running_anim);
	

	projection = projectionMatrix((float)M_PI / 4.0f, screenSize.x / screenSize.y, 0.001f, 1000.0f);

	updateCamera(0.0f, 0.0f);

	bool skyUpdate = true;
	
	float start = getTime();
	//float lastTime = 0.0;

	while (running) {
		handleEvents();
		
		float now = getTime();
		const float ftime = now - start;

		glClear(GL_DEPTH_BUFFER_BIT);
		clearShadow();

		updateLight(ftime, false);
		updateAnimation(ftime);
		
		vec3 characterPosition = {0.0f, 0.0f, 0.0f};
		vec3 reflectionDirection;
		mat4 reflectionView = updateSnow(&reflectionDirection, &projection, &characterModel, &characterPosition);

		if (skyUpdate) updateSky(&lightPosition, ftime, &reflectionDirection);
		skyUpdate = !skyUpdate;

		glViewport(0, 0, (GLsizei)screenSize.x, (GLsizei)screenSize.y);

		switch (currentSceneId) {
			case 0: {
				// Shadow pass
				glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
				glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);

				renderCharacter(shadowCharacterShader, &shadowProjection, &shadowView, &characterModel);
				renderTrees(shadowTreeShader, &shadowProjection, &shadowView, &lightPosition, 0);

				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glViewport(0, 0, (GLsizei)screenSize.x, (GLsizei)screenSize.y);

				// Ice pass
				glBindFramebuffer(GL_FRAMEBUFFER, reflectionFrameBuffer);

				glEnable(GL_STENCIL_TEST);
				glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
				glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

				renderCharacter(characterShader, &projection, &reflectionView, &characterModel);
				renderTrees(treeShader, &projection, &cameraView, &lightPosition, 0);
				renderSky(&projection, &reflectionView);

				glDisable(GL_STENCIL_TEST);

				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				// Main pass
				renderCharacter(characterShader, &projection, &cameraView, &characterModel);
				renderTrees(treeShader, &projection, &cameraView, &lightPosition, 0);
				renderSnow(&projection, &cameraView, &reflectionView, 0);

				break;
			}

			case 1:
				
				break;
		}

		renderSky(&projection, &cameraView);


		checkOpenGLError();
		
		//lastTime = ftime;

		swapBuffers();
	}

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