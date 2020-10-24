#include "scene.h"

void SceneNode::draw(Program * parentProgram, mat4 parentMatrix)
{
	mat4 current = parentMatrix *
		MatrixFactory::translateMat3(transform.position) *
		MatrixFactory::scaleMat4(transform.scale) *
		MatrixFactory::mat3to4(transform.rotation.toRotationMatrix());
	if (program != nullptr) {

		for (SceneNode *node : children)
		{
			node->draw(program, current);
		}
		program->use();
		if (drawable != nullptr) {
			program->setUniformMat4("ModelMatrix", current);
			drawable->draw(program);
		}
	}
	else
	{
		for (SceneNode *node : children)
		{
			node->draw(parentProgram, current);
		}

		parentProgram->use();
		if (drawable != nullptr) {
			parentProgram->setUniformMat4("ModelMatrix", current);
			drawable->draw(parentProgram);
		}
	}
}

SceneNode * SceneNode::createNode(IDrawable *d)
{
	SceneNode *node = createNode();
	node->drawable = d;
	return node;
}

SceneNode * SceneNode::createNode()
{
	SceneNode *node = new SceneNode();
	node->transform.position = { 0,0,0 };
	node->transform.rotation = { 1,{0,0,0} };
	node->transform.scale = { 1,1,1 };
	children.push_back(node);
	return node;
}

SceneNode * Scene::createNode(IDrawable * d)
{
	SceneNode *node = createNode();
	node->drawable = d;
	children.push_back(node);
	return node;
}

SceneNode * Scene::createNode()
{
	SceneNode *node = new SceneNode();
	node->transform.position = { 0,0,0 };
	node->transform.rotation = { 1,{0,0,0} };
	node->transform.scale = { 1,1,1 };
	children.push_back(node);
	return node;
}

void Scene::draw()
{
	mainProgram->use();
	mainProgram->setUniformMat4("ProjectionMatrix", cam->getProjectionMatrix());
	mainProgram->setUniformMat4("ViewMatrix", cam->getViewMatrix());
	for (SceneNode *node : children) {
		node->draw(mainProgram, MatrixFactory::identity4());
	}
}
