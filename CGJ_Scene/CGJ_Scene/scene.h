#ifndef SCENE_H
#define SCENE_H

#include "matrix.h"
#include "quaternions.h"
#include "shaders.h"
#include "vectors.h"
#include "mesh.h"
#include "camera.h"

struct IDrawable {
	virtual void draw(Program *program) = 0;
};

struct Transform {
	vec3 position, scale;
	quaternion rotation;
};

struct SceneNode {
	Program *program = nullptr;
	Transform transform;
	IDrawable *drawable = nullptr;
	vector<SceneNode *> children;
	void draw(Program *parentProgram, mat4 parentMatrix);
	SceneNode *createNode(IDrawable *d);
	SceneNode *createNode();
};

struct Scene
{
	Camera *cam;
	vector<SceneNode *> children;
	Program *mainProgram;
	SceneNode *createNode(IDrawable *d);
	SceneNode *createNode();
	void draw();
};

#endif // !SCENE_H
