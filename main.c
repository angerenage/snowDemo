#include <X11/keysym.h>

#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include "cameraController.h"
#include "shadow.h"
#include "glutils.h"
#include "shader.h"
#include "snow.h"
#include "character.h"
#include "sky.h"
#include "tree.h"

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif

bool running = true;
int currentSceneId = 0;

bool mouseHeld = false;

mat4 projection = {0};

void handleEvents(Display *display, Atom wmDelete) {
	XEvent event;
	while (XPending(display)) {
		XNextEvent(display, &event);
		switch (event.type) {
			case ClientMessage:
				if ((Atom)event.xclient.data.l[0] == wmDelete) {
					running = false;
				}
				break;

			case ConfigureNotify:
				{
					XConfigureEvent xce = event.xconfigure;
					glViewport(0, 0, xce.width, xce.height);

					screenSize = (vec2){(float)xce.width, (float)xce.height};

					projection = projectionMatrix(M_PI / 4.0, (float)xce.width / (float)xce.height, 0.1f, 1000.0f);
				}
				break;

			case KeyPress:
				{
					KeySym key = XLookupKeysym(&event.xkey, 0);
					if (key == XK_Escape) {
						running = false;
					}
					else if (key == XK_Tab) {
						currentSceneId++;
						currentSceneId %= 2;
					}
				}
				break;

			case ButtonPress:
				if (event.xbutton.button == Button1) {
					mouseHeld = true;
					firstMouse = true;
				}
				break;

			case ButtonRelease:
				if (event.xbutton.button == Button1) {
					mouseHeld = false;
				}
				break;

			case MotionNotify:
				if (mouseHeld) updateCamera(event.xmotion.x, event.xmotion.y);
				break;
		}
	}
}

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

	glViewport(0, 0, screenSize.x, screenSize.y);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	Mesh tree = generateTree(10.0f, 0.4f, 10, 100, 3.0f);
	mat4 treeModel = translationMatrix((vec3){0.0f, -5.0f, 0.0f});

	
	mat4 characterModel = rotationMatrix((vec3){0.0f, -M_PI / 2.0f, 0.0f});
	loadAnimation("ressources/running.anim.xz");
	

	projection = projectionMatrix(M_PI / 4.0, screenSize.x / screenSize.y, 0.001f, 1000.0f);

	updateCamera(0.0f, 0.0f);

	bool skyUpdate = true;
	
	struct timespec start;
	clock_gettime(CLOCK_MONOTONIC, &start);
	//float lastTime = 0.0;

	while (running) {
		handleEvents(display, wmDelete);
		
		struct timespec end;
		clock_gettime(CLOCK_MONOTONIC, &end);
		const float ftime = end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9;

		glClear(GL_DEPTH_BUFFER_BIT);
		clearShadow();

		updateLight(ftime);
		updateAnimation(ftime);
		
		vec3 characterPosition = {0.0f, 0.0f, 0.0f};
		vec3 reflectionDirection;
		mat4 reflectionView = updateSnow(&reflectionDirection, &projection, &characterModel, &characterPosition);

		if (skyUpdate) updateSky(&sunPosition, &screenSize, ftime, &reflectionDirection);
		skyUpdate = !skyUpdate;

		glViewport(0, 0, screenSize.x, screenSize.y);

		switch (currentSceneId) {
			case 0: {
				// Shadow pass
				glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
				glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);

				renderCharacter(shadowCharacterShader, &shadowProjection, &shadowView, &characterModel);

				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glViewport(0, 0, screenSize.x, screenSize.y);

				// Ice pass
				glBindFramebuffer(GL_FRAMEBUFFER, reflectionFrameBuffer);

				glEnable(GL_STENCIL_TEST);
				glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
				glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

				renderCharacter(characterShader, &projection, &reflectionView, &characterModel);
				renderSky(&projection, &reflectionView);

				glDisable(GL_STENCIL_TEST);

				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				// Main pass
				renderCharacter(characterShader, &projection, &cameraView, &characterModel);
				renderSnow(&projection, &cameraView, &reflectionView);

				break;
			}

			case 1:
				glUseProgram(debugShader);
				glUniformMatrix4fv(glGetUniformLocation(debugShader, "projection"), 1, GL_FALSE, (GLfloat*)&projection);
				glUniformMatrix4fv(glGetUniformLocation(debugShader, "view"), 1, GL_FALSE, (GLfloat*)&cameraView);
				glUniformMatrix4fv(glGetUniformLocation(debugShader, "model"), 1, GL_FALSE, (GLfloat*)&treeModel);

				glBindVertexArray(tree.VAO);
				glDrawElements(GL_TRIANGLES, tree.indexCount, GL_UNSIGNED_INT, 0);
				break;
		}

		renderSky(&projection, &cameraView);


		checkOpenGLError();
		
		//lastTime = ftime;

		glXSwapBuffers(display, window);
	}

	freeMesh(tree);

	cleanupSnow();
	cleanupCharacter();
	cleanupShadow();
	cleanupSky();
	cleanupUtils();
	cleanupShaders();
	cleanupWindow();

	return 0;
}