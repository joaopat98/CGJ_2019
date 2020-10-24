///////////////////////////////////////////////////////////////////////////////
//
//  Loading OBJ mesh from external file
//
//	Final individual assignment:
//	1.	Create classes: Scene, Camera and Mesh (that loads a mesh from a 
//      Wavefront OBJ file to an indexed format) and build a small scenegraph
//      of your tangram scene (you may create more classes if needed).
//	2.	Create a ground object and couple the tangram figure to the ground. 
//      Press keys to move the ground about: the tangram figure must follow the
//      ground.
//	3.	Animate between closed puzzle (initial square) and tangram figure by
//      pressing a key.
//	4.	Spherical camera interaction through mouse. It should be possible to 
//      interact while animation is playing.
//
// (c) 2013-19 by Carlos Martinho
//
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <reactphysics3d.h>

#include "engine.h"


using namespace std;

GLuint VaoId;

Program program;
Shader vertexShader, fragmentShader;
Scene s;

SceneNode * tangram[7];
Transform poseTangram[7] = {
	{
		{0,0,0},
		{1,1,1},
		{1,{0,0,0}}
	},
	{
		{0,0,0},
		{1,1,1},
		quaternion::fromAxisAngle(90,AXIS3_Y)
	},
	{
		{0,0,-0.25},
		{0.5,0.5,0.5},
		quaternion::fromAxisAngle(180,AXIS3_Y)
	},
	{
		{-0.5,0,-0.25},
		{(float)M_SQRT2 / 2,(float)M_SQRT2 / 2,(float)M_SQRT2 / 2},
		quaternion::fromAxisAngle(-135,AXIS3_Y)
	},
	{
		{0,0,0.25},
		{0.5,0.5,0.5},
		quaternion::fromAxisAngle(-90,AXIS3_Y)
	},
	{
		{-0.5,0,-0.25},
		{0.5,0.5,0.5},
		{1,{0,0,0}}
	},
	{
		{-0.5,0,0.25},
		{0.5,0.5,0.5},
		quaternion::fromAxisAngle(-90,AXIS3_Y)
	}
};
Transform poseSquare[7] = {
	{
		{0,0,0},
		{1,1,1},
		quaternion::fromAxisAngle(-45,AXIS3_Y)
	},
	{
		{0,0,0},
		{1,1,1},
		quaternion::fromAxisAngle(-135,AXIS3_Y)
	},
	{
		{(float)M_SQRT2 / 4,0,(float)M_SQRT2 / 4},
		{0.5,0.5,0.5},
		quaternion::fromAxisAngle(45,AXIS3_Y)
	},
	{
		{(float)M_SQRT2 / 2,0,(float)-M_SQRT2 / 2},
		{(float)M_SQRT2 / 2,(float)M_SQRT2 / 2,(float)M_SQRT2 / 2},
		quaternion::fromAxisAngle(-90,AXIS3_Y)
	},
	{
		{0,0,0},
		{0.5,0.5,0.5},
		quaternion::fromAxisAngle(135,AXIS3_Y)
	},
	{
		{0,0,0},
		{0.5,0.5,0.5},
		quaternion::fromAxisAngle(45,AXIS3_Y)
	},
	{
		{(float)-M_SQRT2 / 4,0,(float)-M_SQRT2 / 4},
		{0.5,0.5,0.5},
		quaternion::fromAxisAngle(-45,AXIS3_Y) * quaternion::fromAxisAngle(180,AXIS3_X)
	}
};

struct ColorMesh : IDrawable {
	Mesh mesh;
	vec4 color;
	ColorMesh(Mesh mesh, vec4 color) {
		this->mesh = mesh;
		this->color = color;
	}
	void draw(Program *program) {
		program->setUniformv4("Color", color);
		mesh.draw();
	}
};

Mesh triangle, paralelogram, square;

/////////////////////////////////////////////////////////////////////// CAMERA

OrthoProjection ortho{ -1,1,-1,1,-20, 20 };
PerspectiveProjection persp{ (float)(M_PI) / 4, 1280 / 720.0f,0.1f,20 };

float mouseX = 0, mouseY = 0;



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

	if (!program.link()) {
		cout << "Error linking program:\n" + program.errorMsg << "press any key to exit...";
		getchar();
		exit(EXIT_FAILURE);
	}

	program.addAttrib("inPosition");
	program.addAttrib("inNormal");

	program.use();

	fragmentShader.destroy();
	vertexShader.destroy();
}

void destroyShaderProgram()
{
	glUseProgram(0);
	program.destroy();
}

/////////////////////////////////////////////////////////////////////// SCENE

int direction = -1;
float time = 0;

void drawScene(double elapsed_sec)
{
	time += (float)elapsed_sec * direction;
	if (time < 0)
		time = 0;
	if (time > 3)
		time = 3;
	if (time < 1)
	{
		for (int i = 0; i < 7; i++)
		{
			tangram[i]->transform.position = lerpV(time, poseTangram[i].position, poseTangram[i].position + (AXIS3_Y * (float)i * 0.2f));
			tangram[i]->transform.rotation = poseTangram[i].rotation;
		}
	}
	else if (time < 2) {
		for (int i = 0; i < 7; i++)
		{
			tangram[i]->transform.position = lerpV(time - 1, poseTangram[i].position, poseSquare[i].position);
			tangram[i]->transform.position += (AXIS3_Y * (float)i * 0.2f);
			tangram[i]->transform.rotation = lerpQ(time - 1, poseTangram[i].rotation, poseSquare[i].rotation);
		}
	}
	else
	{
		for (int i = 0; i < 7; i++)
		{
			tangram[i]->transform.position = lerpV(time - 2, poseSquare[i].position + (AXIS3_Y * (float)i * 0.2f), poseSquare[i].position);
			tangram[i]->transform.rotation = poseSquare[i].rotation;
		}
	}

	s.draw();
}

void setup()
{
	createShaderProgram();
	s.cam = new Camera(6, { 0,{0,1,0} }, &persp);
	s.mainProgram = &program;
	auto ground = s.createNode();
	triangle = Mesh::fromObj(program, "triangle.obj");
	paralelogram = Mesh::fromObj(program, "paralelogram.obj");
	square = Mesh::fromObj(program, "square.obj");
	tangram[0] = ground->createNode(new ColorMesh(triangle, { 1,0,0,1 }));
	tangram[1] = ground->createNode(new ColorMesh(triangle, { 0,1,0,1 }));
	tangram[2] = ground->createNode(new ColorMesh(triangle, { 0,0,1,1 }));
	tangram[3] = ground->createNode(new ColorMesh(triangle, { 1,1,0,1 }));
	tangram[4] = ground->createNode(new ColorMesh(triangle, { 1,0,1,1 }));
	tangram[5] = ground->createNode(new ColorMesh(square, { 0,1,1,1 }));
	tangram[6] = ground->createNode(new ColorMesh(paralelogram, { 1,1,1,1 }));
	for (int i = 0; i < 7; i++)
	{
		tangram[i]->transform = poseTangram[i];
	}

	auto table = ground->createNode(new ColorMesh(Mesh::fromObj(program, "table.obj"), { 0.5,0.5,0.5,1 }));
	table->transform.scale *= 3;
	table->transform.position -= {0.2f, 0.2f, 0.2f};
}

////////////////////////////////////////////////////////////////////////// INPUT

void resize(GLFWwindow* win, int winx, int winy)
{
	persp.aspect = winx / (float)winy;
	glViewport(0, 0, winx, winy);
	// Change projection matrices to maintain aspect ratio
}

void keyboard(GLFWwindow *win, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		if (s.cam->projection == &persp) s.cam->projection = &ortho;
		else s.cam->projection = &persp;
	}
	else if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
		s.cam->distance += 0.5;
	}
	else if (key == GLFW_KEY_E && action == GLFW_PRESS) {
		s.cam->distance -= 0.5;
	}
	else if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
		s.children[0]->transform.rotation = quaternion::fromAxisAngle(10, AXIS3_Y) * s.children[0]->transform.rotation;
	}
	else if (key == GLFW_KEY_X && action == GLFW_PRESS) {
		s.children[0]->transform.rotation = quaternion::fromAxisAngle(-10, AXIS3_Y) * s.children[0]->transform.rotation;
	}
	else if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		s.children[0]->transform.position += AXIS3_Z;
	}
	else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		s.children[0]->transform.position += -AXIS3_X;
	}
	else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		s.children[0]->transform.position += -AXIS3_Z;
	}
	else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		s.children[0]->transform.position += AXIS3_X;
	}
	else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		direction = -direction;
	}
}

void mouse(GLFWwindow *win, double xpos, double ypos) {
	float deltaX = (float)(xpos - mouseX) / 10.0f;
	float deltaY = (float)(ypos - mouseY) / 10.0f;

	s.cam->rotation = (quaternion::fromAxisAngle(deltaY, AXIS3_X)*quaternion::fromAxisAngle(deltaX, AXIS3_Y)*s.cam->rotation).normalize();

	mouseX = (float)(xpos);
	mouseY = (float)(ypos);
}

////////////////////////////////////////////////////////////////////////// MAIN

int main(int argc, char* argv[])
{
	reactphysics3d::WorldSettings ws;
	int gl_major = 4, gl_minor = 3;
	int is_fullscreen = 0;
	int is_vsync = 1;
	Engine e(gl_major, gl_minor,
		1280, 720, "WASD - move table; ZX - rotate table; QE - zoom; Space - toggle animation", is_fullscreen, is_vsync);
	e.setDisplayFn(drawScene);
	e.setKeyFn(keyboard);
	e.setWindowSizeCallBack(resize);
	glfwSetInputMode(e.win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	e.setCursorPosFn(mouse);
	setup();
	e.run();
	program.destroy();
	exit(EXIT_SUCCESS);
}

///////////////////////////////////////////////////////////////////////////////
