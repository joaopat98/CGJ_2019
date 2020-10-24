#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "camera.h"
#include "matrix.h"
#include "quaternions.h"
#include "shaders.h"
#include "vectors.h"
#include "mesh.h"
#include "scene.h"

struct Engine {
	GLFWwindow* win;
	Engine(int major, int minor, int winx, int winy, const char* title, int is_fullscreen, int is_vsync);
	void(*display)(double elapsed_sec);
	void setDisplayFn(void(*fn)(double elapsed_sec));
	void setKeyFn(void(*fn)(GLFWwindow *win, int key, int scancode, int action, int mods));
	void setCursorPosFn(void(*fn)(GLFWwindow *win, double xpos, double ypos));
	void setMouseButtonCallback(void(*fn)(GLFWwindow *win, int button, int action, int mods));
	void setWindowSizeCallBack(void(*fn)(GLFWwindow*, int, int));
	void setScrollCallback(void(*fn)(GLFWwindow *win, double xoffset, double yoffset));
	void setJoystickCallback(void(*fn)(int jid, int event));
	void run();
};

#endif // !ENGINE_H
