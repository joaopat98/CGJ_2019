#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "vectors.h"
#include "matrix.h"
#include "shaders.h"
#include "engine.h"

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
	float matrix[16], color[4];
	for (int i = 0; i < 7; i++)
	{
		glBindVertexArray(vao[shapes[i]]);
		transforms[i].to_GL(matrix);
		colors[i].toArray(color);
		program.setUniformMat4("Matrix", transforms[i].data);
		program.setUniformv4("Color", colors[i]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, numVerts[i]);
	}
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
	createShaderProgram();
	setupShapes();
	e.run();
	destroyBufferObjects();
	program.destroy();
	exit(EXIT_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////// END