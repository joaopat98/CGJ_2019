#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "engine.h"
#include "voronoi.h"
#include "rtt.h"
#include "calcImpact.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <reactphysics3d.h>


#include<iostream>

using namespace engine;
using namespace std;

Engine e;

Program program;
Program skyBoxProgram;
Program glassProgram;
Program woodProgram;
Program marbleProgram;
Program textureProgram;
CubeMap* cubeMap;
Scene s;

struct ColorMesh : IDrawable {
	Mesh mesh;
	vec4 color;
	ColorMesh(Mesh mesh, vec4 color) {
		this->mesh = mesh;
		this->color = color;
	}
	void draw(Program* program) {
		program->setUniformv4("Color", color);
		mesh.draw();
	}
};

struct SkyBoxMesh : IDrawable {
	Mesh mesh;
	CubeMap* cubeMap;
	SkyBoxMesh(Mesh mesh, CubeMap* cubeMap) {
		this->mesh = mesh;
		this->cubeMap = cubeMap;
	}
	void draw(Program* program) {
		glCullFace(GL_FRONT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap->texture);
		mesh.draw();
	}
};

struct GlassMesh : IDrawable {
	Mesh mesh;
	CubeMap* cubeMap;
	GlassMesh(Mesh mesh, CubeMap* cubeMap) {
		this->mesh = mesh;
		this->cubeMap = cubeMap;
	}
	void draw(Program* program) {
		program->setUniformv3("Color", vec3(1, 1, 0));
		program->setUniformv3("viewPos", s.cam->eye);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap->texture);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		mesh.draw();
		glDisable(GL_BLEND);
	}
};

struct FrameMesh : IDrawable {
	Mesh mesh;
	FrameMesh(Mesh mesh) {
		this->mesh = mesh;
	}
	void draw(Program* sphereProgram) {
		sphereProgram->setUniformv3("u_resolution", vec3{ 10, 10, 10 });
		sphereProgram->setUniformv3("viewPos", s.cam->eye);
		sphereProgram->setUniformv3("ambientLight", vec3{ 8.0f, 8.0f, 8.0f });
		sphereProgram->setUniformv3("diffuseLight", vec3{ 2.0f, 2.0f, 2.0f });
		sphereProgram->setUniformv3("specularLight", vec3{ 0.4f, 0.4f, 0.4f });
		sphereProgram->setUniformv3("lightPos", vec3{ 50.0f, 50.0f, 50.0f });
		mesh.draw();
	}
};

struct SphereMesh : IDrawable {
	Mesh mesh;
	SphereMesh(Mesh mesh) {
		this->mesh = mesh;
	}
	void draw(Program* program) {
		program->setUniformv3("u_resolution", vec3{ 10, 10, 10 });
		program->setUniformv3("viewPos", s.cam->eye);
		program->setUniformv3("ambientLight", vec3{ 0.2f, 0.2f, 0.18f });
		program->setUniformv3("diffuseLight", vec3{ 0.7f, 0.7f, 0.65f });
		program->setUniformv3("specularLight", vec3{ 0.1f, 0.1f, 0.09f });
		program->setUniformv3("lightPos", vec3{ 5.0f, 5.0f, 5.0f });
		mesh.draw();
	}
};

struct TextureMesh : IDrawable {
	Mesh mesh;
	Texture texture;
	TextureMesh(Mesh mesh, Texture texture) {
		this->mesh = mesh;
		this->texture = texture;
	}
	void draw(Program* program) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture.texture);
		mesh.draw();
	}
};

Mesh triangle, paralelogram, square;

/////////////////////////////////////////////////////////////////////// CAMERA

OrthoProjection ortho{ -1,1,-1,1,-20, 20 };
PerspectiveProjection persp{ (float)(M_PI) / 4, 1280 / 720.0f,0.1f,1000 };

bool moved = false;
vec3 pos{ -10,10,-10 };
float rx = (float)(-M_PI_4 / 2), ry = (float)M_PI_4;
float mouseX = 0, mouseY = 0;

/////////////////////////////////////////////////////////////////////// SHADERs

void compileShader(Program* program, string vertex, string fragment) {
	program->create();

	auto vertexShader = Shader(vertex, GL_VERTEX_SHADER);
	auto fragmentShader = Shader(fragment, GL_FRAGMENT_SHADER);
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

	program->addShader(vertexShader);
	program->addShader(fragmentShader);

	if (!program->link()) {
		cout << "Error linking program:\n" + program->errorMsg << "press any key to exit...";
		getchar();
		exit(EXIT_FAILURE);
	}

	program->addAttrib("inPosition");
	program->addAttrib("inNormal");
	program->addAttrib("inTexCoord");

	fragmentShader.destroy();
	vertexShader.destroy();
}

void createShaderPrograms()
{
	//// SKYBOX
	compileShader(&skyBoxProgram, "skybox_vertex.glsl", "skybox_fragment.glsl");
	//// GLASS
	compileShader(&glassProgram, "glass_vertex.glsl", "glass_fragment.glsl");
	//// DEBUG
	compileShader(&program, "vertex.glsl", "fragment.glsl");
	//// WOOD
	compileShader(&woodProgram, "wood_vertex.glsl", "wood_fragment.glsl");
	//// MARBLE
	compileShader(&marbleProgram, "marble_vertex.glsl", "marble_fragment.glsl");
	//// TEXTURE
	compileShader(&textureProgram, "texture_vertex.glsl", "texture_fragment.glsl");
}

void destroyShaderProgram()
{
	glUseProgram(0);
	program.destroy();
	glassProgram.destroy();
	woodProgram.destroy();
	textureProgram.destroy();
	marbleProgram.destroy();
	skyBoxProgram.destroy();
}

void createSkyBox()
{
	std::vector<std::string> faces
	{
		"skybox/right.jpg",
			"skybox/left.jpg",
			"skybox/top.jpg",
			"skybox/bottom.jpg",
			"skybox/front.jpg",
			"skybox/back.jpg"
	};
	cubeMap = new CubeMap(faces);
}

/////////////////////////////////////////////////////////////////////// SCENE

SceneNode* sphere;
SceneNode* frame;
SceneNode* glassFather;
SceneNode* tempGlass;
SceneNode* floor_cube;
SceneNode* sky;

bool playing = false;

CubeMap* windowMap, * roomMap;

bool ballHit = false;
float panelScale = 9;
vec3 panelOrigin = vec3{ -4.5f,(float)(-4.5 + 4),15.5f };
vec3 ballVelocity = 1 * vec3{ 0,3,-30 };
float ballRadius = 0.3f;
vec3 ballPos = { 0,5,30 };
float hitThreshold = 1.6f;

float cubemapThreshold = 0.69f;

float panelThickness = 0.1f;

bool compareNodes(SceneNode* n1, SceneNode* n2) {
	float dist1 = (n1->transform.position - s.cam->eye).length();
	float dist2 = (n2->transform.position - s.cam->eye).length();
	return dist1 > dist2;
}

void drawScene(double elapsed_sec)
{
	if (playing)
		s.physicsUpdate((float)elapsed_sec);
	if (!ballHit && sphere->transform.position.z - ballRadius - hitThreshold <= panelOrigin.z + panelThickness) {
		glassFather->enabled = true;
		tempGlass->enabled = false;
		ballHit = true;
		for (auto shard : glassFather->children) {
			if (shard->enabled)
				shard->rigidBody->setType(rp3d::BodyType::DYNAMIC);
			else
			{
				shard->enabled = true;
			}
		}
	}
	sky->transform.position = s.cam->eye;
	sort(glassFather->children.begin(), glassFather->children.end(), compareNodes);
	for (int i = 0; i < glassFather->children.size(); i++)
	{
		GlassMesh* mesh = (GlassMesh*)glassFather->children[i]->drawable;
		mesh->cubeMap = glassFather->children[i]->transform.position.z - panelOrigin.z < - cubemapThreshold ? roomMap : windowMap;
	}
	s.draw();
}

vec3 calcDir() {
	vec3 dir;
	dir.y = sin(rx);
	dir.z = sin(ry) * cos(rx);
	dir.x = cos(ry) * cos(rx);
	return dir;
}

void setupCamera() {
	s.cam = new Camera(pos, pos + calcDir(), { 0,1,0 }, &persp);
}

void setup()
{
	setupCamera();
	createShaderPrograms();
	createSkyBox();
	s.mainProgram = &program;
	s.world = new rp3d::DynamicsWorld(vec3(0.0f, -9.81f, 0.0f));
	cout << s.world->getNbIterationsPositionSolver() << endl;
	cout << s.world->getNbIterationsVelocitySolver() << endl;

	auto skyBox = Mesh::fromObj(skyBoxProgram, "Models/skyBox.obj");
	auto skyBoxMesh = new SkyBoxMesh(skyBox, cubeMap);
	sky = s.createNode(skyBoxMesh);
	sky->program = &skyBoxProgram;
	sky->transform.scale *= 100;

	auto floorTex = Texture("Textures/floor.png");
	auto wallTex = Texture("Textures/wall.png");

	auto floorMesh = Mesh::fromObj(textureProgram, "Models/room_floor.obj");
	auto floorWood = s.createNode(new TextureMesh(floorMesh, floorTex), { {0,-15.5,0},{1,1,1},{1,{0,0,0}} }, &floorMesh, false);
	floorWood->program = &textureProgram;
	floorWood->rigidBody->setType(rp3d::BodyType::KINEMATIC);

	auto wall_L_mesh = Mesh::fromObj(textureProgram, "Models/wall_L.obj");
	auto wall_L = s.createNode(new TextureMesh(wall_L_mesh, wallTex), { {15.5,0,0},{1,1,1},{1,{0,0,0}} }, &wall_L_mesh, false);
	wall_L->program = &textureProgram;
	wall_L->rigidBody->setType(rp3d::BodyType::KINEMATIC);

	auto wall_B_mesh = Mesh::fromObj(textureProgram, "Models/wall_B.obj");
	auto wall_B = s.createNode(new TextureMesh(wall_B_mesh, wallTex), { {0,0,-15.5},{1,1,1},{1,{0,0,0}} }, &wall_B_mesh, false);
	wall_B->program = &textureProgram;
	wall_B->rigidBody->setType(rp3d::BodyType::KINEMATIC);

	auto wall_R_mesh = Mesh::fromObj(textureProgram, "Models/wall_R.obj");
	auto wall_R = s.createNode(new TextureMesh(wall_R_mesh, wallTex), { {-15.5,0,0},{1,1,1},{1,{0,0,0}} }, &wall_R_mesh, false);
	wall_R->program = &textureProgram;
	wall_R->rigidBody->setType(rp3d::BodyType::KINEMATIC);

	auto wall_T_mesh = Mesh::fromObj(textureProgram, "Models/wall_T.obj");
	auto wall_T = s.createNode(new TextureMesh(wall_T_mesh, wallTex), { {0,15.5,0},{1,1,1},{1,{0,0,0}} }, &wall_T_mesh, false);
	wall_T->program = &textureProgram;
	wall_T->rigidBody->setType(rp3d::BodyType::KINEMATIC);

	auto wall_F_mesh = Mesh::fromObj(textureProgram, "Models/wall_F.obj");
	auto wall_F = s.createNode(new TextureMesh(wall_F_mesh, wallTex), { {0,0,31},{1,1,1},{1,{0,0,0}} }, &wall_F_mesh, true);
	wall_F->program = &textureProgram;

	auto frameMesh = Mesh::fromObj(program, "Models/frame.obj");
	frame = s.createNode(new FrameMesh(frameMesh), { {0,4,15.5},{1,1,1},{1,{0,0,0}} }, &frameMesh, true);
	frame->program = &woodProgram;

	s.physicsUpdate(1 / 60.0f);

	createRTTFrameBuffer(2000, 2000);

	PerspectiveProjection perspRTT{ (float)(M_PI) / 2, 1,0.1f,1000 };

	auto old_persp = s.cam->projection;
	s.cam->eye = { 0,4,15.5 };
	s.cam->projection = &perspRTT;

	vector<unsigned char*> images;

	s.cam->up = -AXIS3_Y;
	s.cam->center = s.cam->eye + AXIS3_X;
	images.push_back(getPixels(&s, 1920, 1080));
	s.cam->up = -AXIS3_Y;
	s.cam->center = s.cam->eye - AXIS3_X;
	images.push_back(getPixels(&s, 1920, 1080));
	s.cam->up = AXIS3_Z;
	s.cam->center = s.cam->eye + AXIS3_Y;
	images.push_back(getPixels(&s, 1920, 1080));
	s.cam->up = -AXIS3_Z;
	s.cam->center = s.cam->eye - AXIS3_Y;
	images.push_back(getPixels(&s, 1920, 1080));
	s.cam->up = -AXIS3_Y;
	s.cam->center = s.cam->eye + AXIS3_Z;
	images.push_back(getPixels(&s, 1920, 1080));
	s.cam->center = s.cam->eye - AXIS3_Z;
	images.push_back(getPixels(&s, 1920, 1080));

	windowMap = new CubeMap(images, 2000, 2000);

	s.cam->eye = { 0,0,0 };

	images.clear();

	s.cam->up = -AXIS3_Y;
	s.cam->center = s.cam->eye + AXIS3_X;
	images.push_back(getPixels(&s, 1920, 1080));
	s.cam->up = -AXIS3_Y;
	s.cam->center = s.cam->eye - AXIS3_X;
	images.push_back(getPixels(&s, 1920, 1080));
	s.cam->up = AXIS3_Z;
	s.cam->center = s.cam->eye + AXIS3_Y;
	images.push_back(getPixels(&s, 1920, 1080));
	s.cam->up = -AXIS3_Z;
	s.cam->center = s.cam->eye - AXIS3_Y;
	images.push_back(getPixels(&s, 1920, 1080));
	s.cam->up = -AXIS3_Y;
	s.cam->center = s.cam->eye + AXIS3_Z;
	images.push_back(getPixels(&s, 1920, 1080));
	s.cam->center = s.cam->eye - AXIS3_Z;
	images.push_back(getPixels(&s, 1920, 1080));

	roomMap = new CubeMap(images, 2000, 2000);

	s.cam->up = AXIS3_Y;

	s.cam->projection = old_persp;

	s.cam->eye = pos;
	s.cam->center = s.cam->eye + calcDir();

	Mesh cube = Mesh::fromObj(program, "Models/cube.obj");

	float worldSize = 100;

	Mesh sphereMesh = Mesh::fromObj(program, "Models/sphere.obj");
	sphere = s.createNode(new SphereMesh(sphereMesh), { ballPos,{ballRadius,ballRadius,ballRadius},{1,{0,0,0}} }, &sphereMesh, false);
	sphere->program = &marbleProgram;
	sphere->rigidBody->setLinearVelocity(ballVelocity);
	sphere->rigidBody->setMass(100);

	vector<Mesh> polys;
	vector<vec3> centers;
	vector<bool> isBorder;
	vec2 impact = impactPoint(ballRadius, ballPos, ballVelocity, panelOrigin, panelScale);
	createFragments(glassProgram, panelScale, panelThickness, impact, &polys, &centers, &isBorder);

	glassFather = s.createNode();
	glassFather->enabled = false;
	for (int i = 0; i < polys.size(); i++) {
		auto p = glassFather->createNode(new GlassMesh(polys[i], windowMap), { centers[i] + panelOrigin,{1,1,1},{1,{0,0,0}} }, &polys[i], false);
		p->program = &glassProgram;
		p->rigidBody->setType(rp3d::BodyType::KINEMATIC);
		p->enabled = !isBorder[i];
	}

	tempGlass = s.createNode(new GlassMesh(cube, windowMap));
	tempGlass->transform = { panelOrigin,{panelScale,panelScale,panelThickness},{1,{0,0,0}} };
	tempGlass->program = &glassProgram;
}

////////////////////////////////////////////////////////////////////////// INPUT

void resize(GLFWwindow* win, int winx, int winy)
{
	persp.aspect = winx / (float)winy;
	glViewport(0, 0, winx, winy);
	// Change projection matrices to maintain aspect ratio
}

void keyboard(GLFWwindow* win, int key, int scancode, int action, int mods) {
	vec3 forward = calcDir();
	float moveSpeed = 1;
	vec3 right = forward.cross(-AXIS3_Y);
	if (key == GLFW_KEY_W) {
		s.cam->translate(moveSpeed * forward);
	}
	else if (key == GLFW_KEY_S) {
		s.cam->translate(-moveSpeed * forward);
	}
	else if (key == GLFW_KEY_A) {
		s.cam->translate(moveSpeed * right);
	}
	else if (key == GLFW_KEY_D) {
		s.cam->translate(-moveSpeed * right);
	}
	else if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		if (s.cam->projection == &persp) s.cam->projection = &ortho;
		else s.cam->projection = &persp;
	}
	else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		playing = !playing;
	}
}

void mouse(GLFWwindow* win, double xpos, double ypos) {
	if (moved) {
		float deltaX = (float)(xpos - mouseX);
		float deltaY = (float)(ypos - mouseY);
		ry += deltaX / 300.0f;
		rx -= deltaY / 300.0f;
		if (rx > (float)(0.9 * M_PI / 2)) rx = (float)(0.9 * M_PI / 2);
		if (rx < (float)(-0.9 * M_PI / 2)) rx = (float)(-0.9 * M_PI / 2);
	}
	else {
		moved = true;
	}
	mouseX = (float)(xpos);
	mouseY = (float)(ypos);
	s.cam->center = s.cam->eye + calcDir();
}

////////////////////////////////////////////////////////////////////////// MAIN

int main(int argc, char* argv[])
{
	int gl_major = 4, gl_minor = 3;
	int is_fullscreen = 0;
	int is_vsync = 1;
	e = Engine(gl_major, gl_minor,
		1920, 1080, "WASD - move; Mouse - Look around; SPACE - pause/play simulation", is_fullscreen, is_vsync);
	e.setDisplayFn(drawScene);
	e.setKeyFn(keyboard);
	e.setWindowSizeCallBack(resize);
	glfwSetInputMode(e.win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	e.setCursorPosFn(mouse);
	setup();
	e.run();
	program.destroy();
	delete s.world;
	exit(EXIT_SUCCESS);
}

///////////////////////////////////////////////////////////////////////////////
