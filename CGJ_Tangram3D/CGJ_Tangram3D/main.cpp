///////////////////////////////////////////////////////////////////////////////
//
// Assignment consists in the following:
//
// - Create the following changes to your scene:
//   - Make your TANs double-faced, so they can be seen from both sides.
//   - The new face of each TAN should share the same hue as the original top
//     face color but have a different level of saturation and brightness.
//
// - Add the following functionality (consider a Camera class):
//   - Create a View Matrix from (eye, center, up) parameters.
//   - Create an Orthographic Projection Matrix from (left-right, bottom-top, 
//     near-far) parameters.
//   - Create a Perspective Projection Matrix from (fovy, aspect, nearZ, farZ) 
//     parameters.
//
// - Add the following dynamics to the application:
//   - Create a free 3D camera controlled by the mouse (orientation) and 
//     keyboard (movement) allowing to visualize the scene through all its 
//     angles.
//   - Change perspective from orthographic to perspective and back as
//     a response to pressing the key 'p'.
//
// (c) 2013-19 by Carlos Martinho
//
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "vectors.h"
#include "matrix.h"
#include "shaders.h"
#include "engine.h"
#include "camera.h"

#define VERTICES 0
#define COLORS 1

#define TRIANGLE 0
#define SQUARE 1
#define PARALELOGRAM 2

using namespace std;

GLuint vao[3], vbo[3];
GLuint ProgramId;
Program program;
GLint MatrixUniformId, ColorUniformId;
Shader vertexShader, fragmentShader;
mat4 transforms[7];
vec4 colors[] = {
	{1,1,1,1},
	{1,0,0,1},
	{0,1,0,1},
	{0,0,1,1},
	{1,1,0,1},
	{1,0,1,1},
	{0,1,1,1}
};
int numVerts[] = { 4,3,3,3,3,3,4 };
int shapes[] = { 2, 0, 0, 0, 0, 0, 1 };

/////////////////////////////////////////////////////////////////////// COLOR

vec4 RGBtoHSV(vec4 color) {
	vec4 colorHSV;
	float fCMax = fmax(fmax(color.x, color.y), color.z);
	float fCMin = fmin(fmin(color.x, color.y), color.z);
	float fDelta = fCMax - fCMin;

	if (fDelta > 0) {
		if (fCMax == color.x) {
			colorHSV.x = (float)(60 * (fmod(((color.y - color.z) / fDelta), 6)));
		}
		else if (fCMax == color.y) {
			colorHSV.x = 60 * (((color.z - color.x) / fDelta) + 2);
		}
		else if (fCMax == color.z) {
			colorHSV.x = 60 * (((color.x - color.y) / fDelta) + 4);
		}

		if (fCMax > 0) {
			colorHSV.y = fDelta / fCMax;
		}
		else {
			colorHSV.y = 0;
		}

		colorHSV.z = fCMax;
	}
	else {
		colorHSV.x = 0;
		colorHSV.y = 0;
		colorHSV.z = fCMax;
	}

	if (colorHSV.x < 0) {
		colorHSV.x = 360 + colorHSV.x;
	}
	return colorHSV;
}

vec4 HSVtoRGB(vec4 color) {
	vec4 colorRGB;
	float fC = color.z * color.y;
	float fHPrime = (float)fmod(color.x / 60.0, 6);
	float fX = (float)(fC * (1 - fabs(fmod(fHPrime, 2) - 1)));
	float fM = color.z - fC;

	if (0 <= fHPrime && fHPrime < 1) {
		colorRGB.x = fC;
		colorRGB.y = fX;
		colorRGB.z = 0;
	}
	else if (1 <= fHPrime && fHPrime < 2) {
		colorRGB.x = fX;
		colorRGB.y = fC;
		colorRGB.z = 0;
	}
	else if (2 <= fHPrime && fHPrime < 3) {
		colorRGB.x = 0;
		colorRGB.y = fC;
		colorRGB.z = fX;
	}
	else if (3 <= fHPrime && fHPrime < 4) {
		colorRGB.x = 0;
		colorRGB.y = fX;
		colorRGB.z = fC;
	}
	else if (4 <= fHPrime && fHPrime < 5) {
		colorRGB.x = fX;
		colorRGB.y = 0;
		colorRGB.z = fC;
	}
	else if (5 <= fHPrime && fHPrime < 6) {
		colorRGB.x = fC;
		colorRGB.y = 0;
		colorRGB.z = fX;
	}
	else {
		colorRGB.x = 0;
		colorRGB.y = 0;
		colorRGB.z = 0;
	}

	colorRGB.x += fM;
	colorRGB.y += fM;
	colorRGB.z += fM;

	return colorRGB;
}


/////////////////////////////////////////////////////////////////////// CAMERA

Camera cam;
OrthoProjection ortho{ -1,1,-1,1,-20, 20 };
PerspectiveProjection persp{ (float)(M_PI) / 4,1,0.1f,20 };

vec3 pos;
float rx, ry;
float mouseX = 0, mouseY = 0;

vec3 calcDir() {
	vec3 dir;
	dir.y = sin(rx);
	dir.z = sin(ry) * cos(rx);
	dir.x = cos(ry) * cos(rx);
	return dir;
}

void setupCamera() {
	pos = { 0,0,3 };
	rx = 0;
	ry = (float)(-M_PI / 2);
	cam = Camera(pos, pos + calcDir(), { 0,1,0 }, &persp);
}

/////////////////////////////////////////////////////////////////////// SHADERs

void createShaderProgram()
{
	program.create();

	vertexShader = Shader("vertex.glsl", GL_VERTEX_SHADER);
	fragmentShader = Shader("fragment.glsl", GL_FRAGMENT_SHADER);
	if (!vertexShader.compile()) {
		cout << vertexShader.errorMsg << "press any key to exit...";
		getchar();
		exit(EXIT_FAILURE);
	}
	if (!fragmentShader.compile()) {
		cout << fragmentShader.errorMsg << "press any key to exit...";
		getchar();
		exit(EXIT_FAILURE);
	}

	program.addShader(vertexShader);
	program.addShader(fragmentShader);
	glBindAttribLocation(program.id, VERTICES, "in_Position");

	if (!program.link()) {
		cout << "Error linking program:\n" + program.errorMsg << "press any key to exit...";
		getchar();
		exit(EXIT_FAILURE);
	}
	program.use();

	fragmentShader.destroy();
	vertexShader.destroy();
}

/////////////////////////////////////////////////////////////////////// VAOs & VBOs

void bufferShape(int index, vector<vec4> v) {
	glGenVertexArrays(1, &vao[index]);
	glBindVertexArray(vao[index]);
	{
		glGenBuffers(1, &vbo[index]);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[index]);
		{
			glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(vec4), &v[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(VERTICES);
			glVertexAttribPointer(VERTICES, 4, GL_FLOAT, GL_FALSE, sizeof(vec4), 0);
		}
	}
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void setupShapes() {
	vector<vec4> triangle = { {0,0,0,1}, {0.5,0,0,1}, {0.5,0.5,0,1} };
	vector<vec4> paralelogram = { {0,0,0,1}, {0.5,0,0,1}, {1,0.5,0,1}, {0.5,0.5,0,1} };
	vector<vec4> square = { {0,0,0,1}, {0.5,0,0,1}, {0.5,0.5,0,1}, {0,0.5,0,1} };

	bufferShape(TRIANGLE, triangle);
	bufferShape(SQUARE, square);
	bufferShape(PARALELOGRAM, paralelogram);

	MatrixFactory mf;
	transforms[0] = mf.translateMat3(-0.5, 0, 0) * mf.rotateMat4(90, AXIS3_Z) * mf.scaleMat4(0.5, 0.5, 1);
	transforms[1] = mf.translateMat3(-0.75, -0.25, 0) * mf.rotateMat4(45, AXIS3_Z) * mf.scaleMat4((float)(1 / sqrt(2)), (float)(1 / sqrt(2)), 1);
	transforms[2] = mf.translateMat3(-0.25, 0.625, 0) * mf.rotateMat4(-90, AXIS3_Z);
	transforms[3] = mf.translateMat3(-0.5, 0.25, 0) * mf.scaleMat4(0.5, 0.5, 1);
	transforms[4] = mf.translateMat3(-0.25, -0.25, 0) * mf.rotateMat4(90, AXIS3_Z) * mf.scaleMat4(0.5, 0.5, 1);
	transforms[5] = mf.translateMat3(0.25, 0.125, 0) * mf.rotateMat4(180, AXIS3_Z);
	transforms[6] = mf.translateMat3(-0.5, 0, 0) * mf.scaleMat4(0.5, 0.5, 0);
}

void destroyBufferObjects()
{
	for (int i = 0; i < 3; i++)
	{
		glBindVertexArray(vao[i]);
		glDisableVertexAttribArray(VERTICES);
		glDisableVertexAttribArray(COLORS);
		glDeleteBuffers(1, &vbo[i]);
		glDeleteVertexArrays(1, &vao[i]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

/////////////////////////////////////////////////////////////////////// SCENE

void drawScene(double time)
{
	cout << "ry = " + to_string(ry) + ", rx = " + to_string(rx) + "\n" + cam.eye.toString() + calcDir().toString() + cam.getViewMatrix().to_string();
	cam.center = cam.eye + calcDir();

	program.setUniformMat4("ProjectionMatrix", cam.getProjectionMatrix());
	program.setUniformMat4("ViewMatrix", cam.getViewMatrix());

	glFrontFace(GL_CCW);

	for (int i = 0; i < 7; i++)
	{
		glBindVertexArray(vao[shapes[i]]);
		program.setUniformMat4("ModelMatrix", transforms[i]);
		program.setUniformv4("Color", colors[i]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, numVerts[i]);
	}

	glFrontFace(GL_CW);

	for (int i = 0; i < 7; i++)
	{
		glBindVertexArray(vao[shapes[i]]);
		program.setUniformMat4("ModelMatrix", transforms[i]);
		vec4 colorHSV = RGBtoHSV(colors[i]);
		colorHSV.y = (float)(fmax(colorHSV.y - 0.3f, 0));
		colorHSV.z = (float)(fmax(colorHSV.z - 0.3f, 0));
		program.setUniformv4("Color", HSVtoRGB(colorHSV));
		glDrawArrays(GL_TRIANGLE_FAN, 0, numVerts[i]);
	}
}

////////////////////////////////////////////////////////////////////////// INPUT

void keyboard(GLFWwindow *win, int key, int scancode, int action, int mods) {
	vec3 forward = calcDir();
	vec3 right = forward.cross(-AXIS3_Y);
	if (key == GLFW_KEY_W) {
		cam.translate(0.1f * forward);
	}
	else if (key == GLFW_KEY_S) {
		cam.translate(-0.1f * forward);
	}
	else if (key == GLFW_KEY_A) {
		cam.translate(0.1f * right);
	}
	else if (key == GLFW_KEY_D) {
		cam.translate(-0.1f * right);
	}
	else if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		if (cam.projection == &persp) cam.projection = &ortho;
		else cam.projection = &persp;
	}
}

void mouse(GLFWwindow *win, double xpos, double ypos) {
	float deltaX = (float)(xpos - mouseX);
	float deltaY = (float)(ypos - mouseY);
	ry += deltaX / 300.0f;
	rx -= deltaY / 300.0f;
	if (rx > (float)(0.9 * M_PI / 2)) rx = (float)(0.9 * M_PI / 2);
	if (rx < (float)(-0.9 * M_PI / 2)) rx = (float)(-0.9 * M_PI / 2);
	mouseX = (float)(xpos);
	mouseY = (float)(ypos);
}

////////////////////////////////////////////////////////////////////////// MAIN

int main(int argc, char* argv[])
{
	int gl_major = 4, gl_minor = 3;
	int is_fullscreen = 0;
	int is_vsync = 1;
	Engine e(gl_major, gl_minor,
		500, 500, "Hello Modern 2D World", is_fullscreen, is_vsync);
	e.setDisplayFn(drawScene);
	e.setKeyFn(keyboard);
	glfwSetInputMode(e.win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	e.setCursorPosFn(mouse);
	createShaderProgram();
	setupCamera();
	setupShapes();
	e.run();
	destroyBufferObjects();
	program.destroy();
	exit(EXIT_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////// END
