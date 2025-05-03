#include "../window.h"

#ifndef WIN32

#include <glad/glad.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <time.h>

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif

Display *display;
Window window;
Atom wmDelete;

static XSetWindowAttributes swa;
static GLXContext glc;
static XVisualInfo *vi;

bool running = true;

bool mouseHeld = false;
int currentSceneId = 0;

float getTime() {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec + ts.tv_nsec / 1e9;
}

void initWindow(vec2 size) {
	display = XOpenDisplay(NULL);
#ifdef DEBUG
	if (display == NULL) {
		fprintf(stderr, "Cannot open display\n");
		exit(EXIT_FAILURE);
	}
#endif

	int screen = DefaultScreen(display);
	Window root = RootWindow(display, screen);

	int fbAttribs[] = {
		GLX_X_RENDERABLE, True,
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_RENDER_TYPE, GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_ALPHA_SIZE, 8,
		GLX_DEPTH_SIZE, 24,
		GLX_STENCIL_SIZE, 8,
		GLX_DOUBLEBUFFER, True,
		GLX_SAMPLE_BUFFERS, 1,
		GLX_SAMPLES, 4,
		None
	};

	int fbcount;
	GLXFBConfig *fbConfigs = glXChooseFBConfig(display, screen, fbAttribs, &fbcount);
#ifdef DEBUG
	if (!fbConfigs || fbcount == 0) {
		fprintf(stderr, "Failed to retrieve framebuffer config\n");
		exit(EXIT_FAILURE);
	}
#endif

	GLXFBConfig fbConfig = fbConfigs[0];
	XFree(fbConfigs);

	vi = glXGetVisualFromFBConfig(display, fbConfig);
#ifdef DEBUG
	if (vi == NULL) {
		fprintf(stderr, "No appropriate visual found\n");
		exit(EXIT_FAILURE);
	}
#endif

	swa.colormap = XCreateColormap(display, root, vi->visual, AllocNone);
	swa.border_pixel = 0;
	swa.event_mask = KeyPressMask | StructureNotifyMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask;

	window = XCreateWindow(
		display, root,
		0, 0, size.x, size.y, 0,
		vi->depth, InputOutput,
		vi->visual,
		CWBorderPixel | CWColormap | CWEventMask, &swa
	);

	wmDelete = XInternAtom(display, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(display, window, &wmDelete, 1);
	
#ifdef FULLSCREEN
	Atom wm_state = XInternAtom(display, "_NET_WM_STATE", False);
	Atom fullscreen = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);
	Atom xa_atom = XInternAtom(display, "ATOM", False);

	XChangeProperty(display, window, wm_state, xa_atom, 32, PropModeReplace, (unsigned char *)&fullscreen, 1);
#endif

	XMapWindow(display, window);
	XStoreName(display, window, "Snow Demo");

	typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
	glXCreateContextAttribsARBProc glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc) glXGetProcAddressARB((const GLubyte *) "glXCreateContextAttribsARB");
#ifdef DEBUG
	if (glXCreateContextAttribsARB == NULL) {
		fprintf(stderr, "glXCreateContextAttribsARB not found. Exiting.\n");
		exit(EXIT_FAILURE);
	}
#endif

	int contextAttribs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
		GLX_CONTEXT_MINOR_VERSION_ARB, 3,
		GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		None
	};

	glc = glXCreateContextAttribsARB(display, fbConfig, NULL, True, contextAttribs);
#ifdef DEBUG
	if (!glc) {
		fprintf(stderr, "Failed to create GL context\n");
		exit(EXIT_FAILURE);
	}
#endif
	glXMakeCurrent(display, window, glc);

#ifdef DEBUG
	if (!gladLoadGLLoader((GLADloadproc)glXGetProcAddress)) {
		fprintf(stderr, "Failed to initialize GLAD\n");
		cleanupWindow();
		exit(EXIT_FAILURE);
	}
#else
	gladLoadGLLoader((GLADloadproc)glXGetProcAddress);
#endif

	screenSize = size;
	projection = projectionMatrix(M_PI / 4.0, size.x / size.y, 0.1f, 1000.0f);
}

void handleEvents() {
	XEvent event;
	while (XPending(display)) {
		XNextEvent(display, &event);
		switch (event.type) {
			case ClientMessage:
				if ((Atom)event.xclient.data.l[0] == wmDelete)
					running = false;
				break;

			case ConfigureNotify: {
				XConfigureEvent xce = event.xconfigure;
				glViewport(0, 0, xce.width, xce.height);
				screenSize = (vec2){(float)xce.width, (float)xce.height};
				projection = projectionMatrix(M_PI / 4.0, screenSize.x / screenSize.y, 0.1f, 1000.0f);
				break;
			}

			case KeyPress: {
				KeySym key = XLookupKeysym(&event.xkey, 0);
				if (key == XK_Escape)
					running = false;
				else if (key == XK_Tab)
					currentSceneId = (currentSceneId + 1) % 2;
				break;
			}

			case ButtonPress:
				if (event.xbutton.button == Button1) {
					mouseHeld = true;
					firstMouse = true;
				}
				break;

			case ButtonRelease:
				if (event.xbutton.button == Button1)
					mouseHeld = false;
				break;

			case MotionNotify:
				if (mouseHeld)
					moveCamera(event.xmotion.x, event.xmotion.y);
				break;
		}
	}
}

void swapBuffers() {
	glXSwapBuffers(display, window);
}

void cleanupWindow() {
	glXMakeCurrent(display, None, NULL);
	glXDestroyContext(display, glc);
	XDestroyWindow(display, window);
	XFreeColormap(display, swa.colormap);
	XFree(vi);
	XCloseDisplay(display);
}

#endif