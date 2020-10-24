#ifndef SCENE_H
#define SCENE_H

#include <reactphysics3d.h>

#include "matrix.h"
#include "quaternions.h"
#include "shaders.h"
#include "vectors.h"
#include "mesh.h"
#include "camera.h"

namespace rp3d = reactphysics3d;

struct IDrawable {
	virtual void draw(Program* program) = 0;
};

struct Transform {
	vec3 position, scale;
	quaternion rotation;

	static Transform fromPhysicsTransform(rp3d::Transform t, vec3 s);
	rp3d::Transform toPhysicsTransform();

	operator rp3d::Transform();
};

struct SceneNode {
	bool enabled;
	float* _vertices;
	int* _indices;
	rp3d::TriangleMesh _tMesh;
	Program* program = nullptr;
	Transform transform;
	rp3d::DynamicsWorld* _world;

	rp3d::ProxyShape* proxyShape;
	rp3d::RigidBody* rigidBody;

	IDrawable* drawable = nullptr;
	vector<SceneNode*> children;
	void draw(Program* parentProgram, mat4 viewMatrix, mat4 projMatrix, mat4 parentMatrix);
	SceneNode* createNode(IDrawable* d);
	SceneNode* createNode();

	void updateTransform(float remainingFactor);
	void setTransform(Transform t);
	void setPosition(vec3 pos);
	void setRotation(quaternion rot);
	SceneNode* createNode(IDrawable* d, Transform initT, Mesh* mesh, bool isScenery);
};

struct Scene
{
	double _timeAcum = 0;

	Camera* cam = nullptr;
	vector<SceneNode*> children;
	rp3d::DynamicsWorld* world = nullptr;
	Program* mainProgram = nullptr;
	SceneNode* createNode(IDrawable* d);
	SceneNode* createNode();
	SceneNode* createNode(IDrawable* d, Transform initT, Mesh* Mesh, bool isScenery);

	void physicsUpdate(float deltaTime);
	void draw();
};

#endif // !SCENE_H
