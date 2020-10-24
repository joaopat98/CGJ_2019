#ifndef RTT_H
#define RRT_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "scene.h"

void createRTTFrameBuffer(int width, int height);
unsigned char* getPixels(Scene* s, int old_w, int old_h);

#endif