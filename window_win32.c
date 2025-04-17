#include "window.h"

#ifdef WIN32

#include <windows.h>
#include <windowsx.h>
#include <glad/glad.h>
#include <GL/wglext.h>

static HWND hwindow;
static HDC hdc;
static HGLRC hglrc;

bool running = true;
int currentSceneId = 0;
mat4 projection;

static bool mouseHeld = false;

float getTime() {
	LARGE_INTEGER freq, counter;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&counter);
	return (float)counter.QuadPart / freq.QuadPart;
}

extern int main(void);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	(void)hInstance;
	(void)hPrevInstance;
	(void)lpCmdLine;
	(void)nCmdShow;
	return main();
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_PAINT:
			ValidateRect(hwnd, NULL);
			return 0;

		case WM_DESTROY:
			running = false;
			PostQuitMessage(0);
			return 0;

		case WM_SIZE: {
			int width = LOWORD(lParam);
			int height = HIWORD(lParam);
			glViewport(0, 0, width, height);
			screenSize = (vec2){ (float)width, (float)height };
			projection = projectionMatrix((float)M_PI / 4.0f, screenSize.x / screenSize.y, 0.1f, 1000.0f);
			break;
		}

		case WM_KEYDOWN:
			switch (wParam) {
				case VK_ESCAPE:
					running = false;
					break;
				case VK_TAB:
					currentSceneId = (currentSceneId + 1) % 2;
					break;
			}
			break;

		case WM_LBUTTONDOWN:
			mouseHeld = true;
			firstMouse = true;
			break;

		case WM_LBUTTONUP:
			mouseHeld = false;
			break;

		case WM_MOUSEMOVE:
			if (mouseHeld) {
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);
				updateCamera((float)x, (float)y);
			}
			break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

static PROC getWGLProc(const char *name) {
	HMODULE opengl32 = LoadLibraryA("opengl32.dll");
	PROC p = wglGetProcAddress(name);
	if (!p) p = GetProcAddress(opengl32, name);
	return p;
}

void initWindow(vec2 size) {
	const char *CLASS_NAME = "SnowWindowClass";

	WNDCLASS wc = {0};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = CLASS_NAME;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	RegisterClass(&wc);

	HWND tempHwnd = CreateWindowEx(0, CLASS_NAME, "Temp", WS_OVERLAPPEDWINDOW, 0, 0, 1, 1, NULL, NULL, wc.hInstance, NULL);
	if (!tempHwnd) {
		MessageBoxA(NULL, "Failed to create temp window", "Error", MB_ICONERROR);
		exit(1);
	}
	HDC tempDC = GetDC(tempHwnd);

	PIXELFORMATDESCRIPTOR pfd = {
		.nSize = sizeof(pfd),
		.nVersion = 1,
		.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		.iPixelType = PFD_TYPE_RGBA,
		.cColorBits = 32,
		.cDepthBits = 24,
		.cStencilBits = 8,
		.iLayerType = PFD_MAIN_PLANE
	};

	int tempPixelFormat = ChoosePixelFormat(tempDC, &pfd);
	SetPixelFormat(tempDC, tempPixelFormat, &pfd);
	HGLRC tempContext = wglCreateContext(tempDC);
	if (!tempContext) {
		MessageBoxA(NULL, "Failed to create temp GL context", "Error", MB_ICONERROR);
		exit(1);
	}
	wglMakeCurrent(tempDC, tempContext);

	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB =
		(PFNWGLCHOOSEPIXELFORMATARBPROC)getWGLProc("wglChoosePixelFormatARB");
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB =
		(PFNWGLCREATECONTEXTATTRIBSARBPROC)getWGLProc("wglCreateContextAttribsARB");

	if (!wglChoosePixelFormatARB || !wglCreateContextAttribsARB) {
		MessageBoxA(NULL, "Missing WGL extensions", "Error", MB_ICONERROR);
		exit(1);
	}

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(tempContext);
	ReleaseDC(tempHwnd, tempDC);
	DestroyWindow(tempHwnd);

	hwindow = CreateWindowEx(
		0, CLASS_NAME, "Snow Demo", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, (int)size.x, (int)size.y,
		NULL, NULL, wc.hInstance, NULL
	);
	if (!hwindow) {
		MessageBoxA(NULL, "Failed to create real window", "Error", MB_ICONERROR);
		exit(1);
	}
	hdc = GetDC(hwindow);

	int pixelAttribs[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		WGL_SAMPLE_BUFFERS_ARB, 1,
		WGL_SAMPLES_ARB, 4,
		0
	};

	int pixelFormat;
	UINT numFormats;
	if (!wglChoosePixelFormatARB(hdc, pixelAttribs, NULL, 1, &pixelFormat, &numFormats) || numFormats == 0) {
		MessageBoxA(NULL, "Failed to choose pixel format", "Error", MB_ICONERROR);
		exit(1);
	}
	DescribePixelFormat(hdc, pixelFormat, sizeof(pfd), &pfd);
	SetPixelFormat(hdc, pixelFormat, &pfd);

	const int attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	hglrc = wglCreateContextAttribsARB(hdc, 0, attribs);
	if (!hglrc) {
		MessageBoxA(NULL, "Failed to create final GL context", "Error", MB_ICONERROR);
		exit(1);
	}
	wglMakeCurrent(hdc, hglrc);

	if (!gladLoadGLLoader((GLADloadproc)getWGLProc)) {
		MessageBoxA(NULL, "Failed to initialize GLAD", "Error", MB_ICONERROR);
		exit(1);
	}

	ShowWindow(hwindow, SW_SHOW);
	UpdateWindow(hwindow);

	running = true;
	screenSize = size;
	projection = projectionMatrix((float)M_PI / 4.0f, size.x / size.y, 0.1f, 1000.0f);
}

void handleEvents() {
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void swapBuffers() {
	SwapBuffers(hdc);
}

void cleanupWindow() {
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hglrc);
	ReleaseDC(hwindow, hdc);
	DestroyWindow(hwindow);
}

#endif