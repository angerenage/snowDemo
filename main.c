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

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif

vec2 screenSize = {800.0, 600.0};

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

int main(int argc, char *argv[]) {
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

	glViewport(0, 0, screenSize.x, screenSize.y);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	
	loadAnimation("ressources/running.anim.xz");
	

	const int chunkNbr = 3;
	const float chunkSize = 25.0f;

	const Mesh terrainMesh = generateGrid((vec2){chunkSize, chunkSize}, 200, -4.0f);

	GLuint terrainHeights[chunkNbr][chunkNbr];
	for (int x = 0; x < chunkNbr; x++) {
		for (int z = 0; z < chunkNbr; z++) {
			terrainHeights[x][z] = generateTerrainHeight(&(vec2){(float)x, (float)z});
		}
	}

	
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

		if (skyUpdate) updateSky(&sunPosition, &screenSize, ftime);
		skyUpdate = !skyUpdate;

		switch (currentSceneId) {
			case 0: {
				const float mapCenter = (chunkNbr - 1) / 2.0f;

				//Shadow pass
				glDisable(GL_CULL_FACE);

				glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
				glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);

				glUseProgram(shadowShader);

				glUniformMatrix4fv(glGetUniformLocation(shadowShader, "projection"), 1, GL_FALSE, (GLfloat*)&shadowProjection);
				glUniformMatrix4fv(glGetUniformLocation(shadowShader, "view"), 1, GL_FALSE, (GLfloat*)&shadowView);
				glUniform1f(glGetUniformLocation(shadowShader, "size"), chunkSize);
				
				for (int x = 0; x < chunkNbr; x++) {
					for (int z = 0; z < chunkNbr; z++) {
						mat4 model = translationMatrix((vec3){((float)x - mapCenter) * chunkSize, 0.0, ((float)z - mapCenter) * chunkSize});

						glUniformMatrix4fv(glGetUniformLocation(shadowShader, "model"), 1, GL_FALSE, (GLfloat*)&model);

						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, terrainHeights[x][z]);
						glUniform1i(glGetUniformLocation(shadowShader, "terrainTex"), 0);

						glBindVertexArray(terrainMesh.VAO);
						glDrawElements(GL_TRIANGLES, terrainMesh.indexCount, GL_UNSIGNED_INT, 0);
					}
				}

				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glViewport(0, 0, screenSize.x, screenSize.y);

				glEnable(GL_CULL_FACE);

				//Render pass
				glUseProgram(terrainShader);

				glUniformMatrix4fv(glGetUniformLocation(terrainShader, "projection"), 1, GL_FALSE, (GLfloat*)&projection);
				glUniformMatrix4fv(glGetUniformLocation(terrainShader, "view"), 1, GL_FALSE, (GLfloat*)&view);
				glUniformMatrix4fv(glGetUniformLocation(terrainShader, "shadowProjection"), 1, GL_FALSE, (GLfloat*)&shadowProjection);
				glUniformMatrix4fv(glGetUniformLocation(terrainShader, "shadowView"), 1, GL_FALSE, (GLfloat*)&shadowView);
				glUniform3fv(glGetUniformLocation(terrainShader, "sunPos"), 1, (GLfloat*)&sunPosition);
				glUniform1f(glGetUniformLocation(terrainShader, "size"), chunkSize);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, shadowMap);
				glUniform1i(glGetUniformLocation(terrainShader, "shadowMap"), 0);

				for (int x = 0; x < chunkNbr; x++) {
					for (int z = 0; z < chunkNbr; z++) {
						mat4 model = translationMatrix((vec3){((float)x - mapCenter) * chunkSize, 0.0, ((float)z - mapCenter) * chunkSize});

						glUniformMatrix4fv(glGetUniformLocation(terrainShader, "model"), 1, GL_FALSE, (GLfloat*)&model);

						glActiveTexture(GL_TEXTURE1);
						glBindTexture(GL_TEXTURE_2D, terrainHeights[x][z]);
						glUniform1i(glGetUniformLocation(terrainShader, "terrainTex"), 1);

						glBindVertexArray(terrainMesh.VAO);
						glDrawElements(GL_TRIANGLES, terrainMesh.indexCount, GL_UNSIGNED_INT, 0);
					}
				}

				glBindVertexArray(0);
				break;
			}

			case 1:
				renderCharacter(projection, view, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 0.0f, 0.0f});
				break;
		}

		renderSky(&projection);


		checkOpenGLError();
		
		//lastTime = ftime;

		glXSwapBuffers(display, window);
	}

	freeMesh(terrainMesh);

	for (int i = 0; i < chunkNbr; i++) {
		glDeleteTextures(chunkNbr, terrainHeights[i]);
	}
	
	cleanupCharacter();
	cleanupShadow();
	cleanupSky();
	cleanupUtils();
	cleanupShaders();
	cleanupWindow();

	return 0;
}