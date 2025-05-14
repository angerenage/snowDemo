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
#include "text.h"

#define SCENE1_TIME 55.0f
#define SCENE2_TIME 25.0f

#define TRANSITION_TIME 3.0f

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

	Text openingText = createText(text_opening, 0.08f);

	Text creditText = createText(text_credit, 0.065f);
	Text musicTitleText = createText(text_music_title, 0.08f);
	Text musicCreditText = createText(text_music_credit, 0.06f);
	Text thanksTitleText = createText(text_thanks_title, 0.08f);
	Text thanksParis8Text = createText(text_thanks_paris8, 0.055f);
	Text thanksElseText = createText(text_thanks_else, 0.055f);

	Text *credits[] = {
		&creditText,
		&musicTitleText,
		&musicCreditText,
		&thanksTitleText,
		&thanksParis8Text,
		&thanksElseText,
	};

	double start = getTime();
	bool first = true;

	while (running) {
		handleEvents();

		double now = getTime();
		float ftime = (float)(now - start);

		glClear(GL_DEPTH_BUFFER_BIT);

		if (currentSceneId < 0) {
			glClear(GL_COLOR_BUFFER_BIT);

			fixHorizontal(&openingText, CENTER_ANCHOR, 0.0);
			fixVertical(&openingText, BOTTOM_ANCHOR, 100.0);

			mat4 model = transformMatrix(openingText.pos, (vec3){0.0f, 0.0f, 0.0f}, (vec3){openingText.scale, openingText.scale, openingText.scale});

			glUseProgram(textShader);

			glUniformMatrix4fv(glGetUniformLocation(textShader, uniform_model), 1, GL_FALSE, (GLfloat*)&model);
			glUniform1f(glGetUniformLocation(textShader, uniform_aspectRatio), screenSize.x / screenSize.y);
			glUniform1f(glGetUniformLocation(textShader, uniform_time), ftime);

			glBindVertexArray(openingText.mesh.VAO);
			glDrawElements(GL_TRIANGLES, openingText.mesh.indexCount, GL_UNSIGNED_INT, NULL);

			swapBuffers();
			continue;
		}

		if (first) {
			first = false;
			start = now;
			ftime = 0.0f;
		}

		updateLight(ftime, currentSceneId != 0);
		updateCharacter(ftime);

		if (currentSceneId == 0) {
			glClear(GL_STENCIL_BUFFER_BIT);

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

					characterModel = transformMatrix((vec3){-5.0f, -0.1f, 0.9f}, (vec3){0.0f, (float)M_PI / 6.0f, 0.0f}, (vec3){1.0f, 1.0f, 1.0f});

					loadAnimation(&res_idle_anim);

					start = now;

					glDisable(GL_STENCIL_TEST);

					updateCamera();
				}
			}
		}
		else if (currentSceneId == 1) {
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

			if (ftime > SCENE2_TIME) {
				currentSceneId++;
				start = now;
			}
		}
		else {
			glClear(GL_COLOR_BUFFER_BIT);

			fixHorizontal(&creditText, CENTER_ANCHOR, 0.0f);
			fixVertical(&creditText, TOP_ANCHOR, screenSize.y * 0.175f - 25.0f);

			fixHorizontal(&musicTitleText, CENTER_ANCHOR, 0.0f);
			fixVertical(&musicTitleText, TOP_ANCHOR, screenSize.y * 0.5f);

			fixHorizontal(&musicCreditText, CENTER_ANCHOR, 0.0f);
			fixVertical(&musicCreditText, TOP_ANCHOR, screenSize.y * 0.7f);

			fixHorizontal(&thanksTitleText, CENTER_ANCHOR, 0.0f);
			fixVertical(&thanksTitleText, MIDDLE_ANCHOR, screenSize.y * -0.1f);

			fixHorizontal(&thanksParis8Text, CENTER_ANCHOR, 0.0f);
			fixVertical(&thanksParis8Text, MIDDLE_ANCHOR, screenSize.y * -0.3f);

			fixHorizontal(&thanksElseText, CENTER_ANCHOR, 0.0f);
			fixVertical(&thanksElseText, MIDDLE_ANCHOR, screenSize.y * -0.5f);

			for (unsigned int i = 0; i < sizeof(credits) / sizeof(Text*); i++) {
				mat4 model = transformMatrix(credits[i]->pos, (vec3){0.0f, 0.0f, 0.0f}, (vec3){credits[i]->scale, credits[i]->scale, credits[i]->scale});
				
				glUseProgram(textShader);

				glUniformMatrix4fv(glGetUniformLocation(textShader, uniform_model), 1, GL_FALSE, (GLfloat*)&model);
				glUniform1f(glGetUniformLocation(textShader, uniform_aspectRatio), screenSize.x / screenSize.y);
				glUniform1f(glGetUniformLocation(textShader, uniform_time), ftime - (i * 0.4f));

				glBindVertexArray(credits[i]->mesh.VAO);
				glDrawElements(GL_TRIANGLES, credits[i]->mesh.indexCount, GL_UNSIGNED_INT, NULL);
			}
		}

#ifdef DEBUG
		checkOpenGLError();
#endif

		swapBuffers();
	}

	for (unsigned int i = 0; i < sizeof(credits) / sizeof(Text*); i++) freeMesh(credits[i]->mesh);
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