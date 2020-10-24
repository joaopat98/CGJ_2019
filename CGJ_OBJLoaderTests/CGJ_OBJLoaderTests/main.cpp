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

#include "engine.h"

bool TexcoordsLoaded, NormalsLoaded;

using namespace std;

GLuint VaoId;

Program program;
Shader vertexShader, fragmentShader;

/////////////////////////////////////////////////////////////////////// CAMERA

OrthoProjection ortho{ -1,1,-1,1,-20, 20 };
PerspectiveProjection persp{ (float)(M_PI) / 4,1,0.1f,20 };
Projection *proj = &persp;

bool gimbal_lock = false;

float mouseX = 0, mouseY = 0;

float d = 8;

quaternion q{ 0,{0,1,0} };
mat4 rx = MatrixFactory::identity4();
mat4 ry = MatrixFactory::identity4();

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
	if (TexcoordsLoaded)
		program.addAttrib("inTexcoord");
	if (NormalsLoaded)
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

////////////////////////////////////////////////////////////////////////// MESH

vector <vec3> Vertices, vertexData;
vector <vec2> Texcoords, texcoordData;
vector <vec3> Normals, normalData;

vector <unsigned int> vertexIdx, texcoordIdx, normalIdx;

void parseVertex(stringstream& sin)
{
	vec3 v;
	sin >> v.x >> v.y >> v.z;
	vertexData.push_back(v);
}

void parseTexcoord(stringstream& sin)
{
	vec2 t;
	sin >> t.x >> t.y;
	texcoordData.push_back(t);
}

void parseNormal(stringstream& sin)
{
	vec3 n;
	sin >> n.x >> n.y >> n.z;
	normalData.push_back(n);
}

void parseFace(stringstream& sin)
{
	string token;
	if (normalData.empty() && texcoordData.empty())
	{
		for (int i = 0; i < 3; i++)
		{
			sin >> token;
			vertexIdx.push_back(stoi(token));
		}
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			getline(sin, token, '/');
			if (token.size() > 0) vertexIdx.push_back(stoi(token));
			getline(sin, token, '/');
			if (token.size() > 0) texcoordIdx.push_back(stoi(token));
			getline(sin, token, ' ');
			if (token.size() > 0) normalIdx.push_back(stoi(token));
		}
	}
}

void parseLine(stringstream& sin)
{
	string s;
	sin >> s;
	if (s.compare("v") == 0) parseVertex(sin);
	else if (s.compare("vt") == 0) parseTexcoord(sin);
	else if (s.compare("vn") == 0) parseNormal(sin);
	else if (s.compare("f") == 0) parseFace(sin);
}

void loadMeshData(string& filename)
{
	ifstream ifile(filename);
	string line;
	while (getline(ifile, line)) {
		stringstream sline(line);
		parseLine(sline);
	}
	TexcoordsLoaded = (texcoordIdx.size() > 0);
	NormalsLoaded = (normalIdx.size() > 0);
}

void processMeshData()
{
	for (unsigned int i = 0; i < vertexIdx.size(); i++) {
		unsigned int vi = vertexIdx[i];
		vec3 v = vertexData[vi - 1];
		Vertices.push_back(v);
		if (TexcoordsLoaded) {
			unsigned int ti = texcoordIdx[i];
			vec2 t = texcoordData[ti - 1];
			Texcoords.push_back(t);
		}
		if (NormalsLoaded) {
			unsigned int ni = normalIdx[i];
			vec3 n = normalData[ni - 1];
			Normals.push_back(n);
		}
	}
}

void freeMeshData()
{
	vertexData.clear();
	texcoordData.clear();
	normalData.clear();
	vertexIdx.clear();
	texcoordIdx.clear();
	normalIdx.clear();
}

const void createMesh(string& filename)
{
	loadMeshData(filename);
	processMeshData();
	freeMeshData();
}

/////////////////////////////////////////////////////////////////////// VAOs & VBOs

void createBufferObjects()
{
	GLuint VboVertices, VboTexcoords, VboNormals;

	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);
	{
		glGenBuffers(1, &VboVertices);
		glBindBuffer(GL_ARRAY_BUFFER, VboVertices);
		glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(vec3), &Vertices[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(program.attribs["inPosition"]);
		glVertexAttribPointer(program.attribs["inPosition"], 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);

		if (TexcoordsLoaded)
		{
			glGenBuffers(1, &VboTexcoords);
			glBindBuffer(GL_ARRAY_BUFFER, VboTexcoords);
			glBufferData(GL_ARRAY_BUFFER, Texcoords.size() * sizeof(vec2), &Texcoords[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(program.attribs["inTexcoord"]);
			glVertexAttribPointer(program.attribs["inTexcoord"], 2, GL_FLOAT, GL_FALSE, sizeof(vec2), 0);
		}
		if (NormalsLoaded)
		{
			glGenBuffers(1, &VboNormals);
			glBindBuffer(GL_ARRAY_BUFFER, VboNormals);
			glBufferData(GL_ARRAY_BUFFER, Normals.size() * sizeof(vec3), &Normals[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(program.attribs["inNormal"]);
			glVertexAttribPointer(program.attribs["inNormal"], 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
		}
	}
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void destroyBufferObjects()
{
	glBindVertexArray(VaoId);
	program.removeAttrib("inPosition");
	program.removeAttrib("inTexcoord");
	program.removeAttrib("inNormal");
	glDeleteVertexArrays(1, &VaoId);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

/////////////////////////////////////////////////////////////////////// SCENE

void drawScene(double elapsed_sec)
{
	glBindVertexArray(VaoId);
	glFrontFace(GL_CW);
	cout << q.toString();
	//cout << "quaternion = " + q.toString() + "quaternion matrix:\n" + MatrixFactory::mat3to4(q.toRotationMatrix()).to_string() + "rotation:\n" + (rx*ry).to_string();
	program.setUniformMat4("ProjectionMatrix", proj->getProjectionMatrix());
	program.setUniformMat4("ViewMatrix", MatrixFactory::translateMat3(0, 0, d) * (
		gimbal_lock ? rx * ry : MatrixFactory::mat3to4(q.toRotationMatrix())
		));
	program.setUniformMat4("ModelMatrix", MatrixFactory::identity4());
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)Vertices.size());

	glBindVertexArray(0);
}

void setup()
{
	//	string mesh_file = "cube_v.obj";
	//	string mesh_file = "cube_vn.obj";
	//	string mesh_file = "cube_vtn.obj";
	//	string mesh_file = "torus_vtn_flat.obj";
	//	string mesh_file = "torus_vtn_smooth.obj";
	string mesh_file = "bunny.obj";
	//	string mesh_file = "suzanne_vtn_smooth.obj";
	//	string mesh_file = "teapot_vn_flat.obj";
	//	string mesh_file = "teapot_vn_smooth.obj";
	//	string mesh_file = "bunny_vn_flat.obj";
	//	string mesh_file = "bunny_vn_smooth.obj";
	string mesh_fullname = mesh_file;
	createMesh(mesh_fullname);
	createShaderProgram();
	createBufferObjects();
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
		if (proj == &persp) proj = &ortho;
		else proj = &persp;
	}
	if (key == GLFW_KEY_G && action == GLFW_PRESS)
	{
		gimbal_lock = !gimbal_lock;
	}
}

void mouse(GLFWwindow *win, double xpos, double ypos) {
	float deltaX = (float)(xpos - mouseX) / 10.0f;
	float deltaY = (float)(ypos - mouseY) / 10.0f;

	rx = MatrixFactory::rotateMat4(deltaY, AXIS3_X) * rx;
	ry = MatrixFactory::rotateMat4(deltaX, AXIS3_Y) * ry;

	q = (quaternion::fromAxisAngle(deltaY, AXIS3_X)*quaternion::fromAxisAngle(deltaX, AXIS3_Y)*q).normalize();

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
	e.setWindowSizeCallBack(resize);
	glfwSetInputMode(e.win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	e.setCursorPosFn(mouse);
	setup();
	e.run();
	destroyBufferObjects();
	program.destroy();
	exit(EXIT_SUCCESS);
}

///////////////////////////////////////////////////////////////////////////////
