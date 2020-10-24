#include "scene.h"

double physicsTimeStep = 1 / 60.0;

void SceneNode::draw(Program* parentProgram, mat4 viewMatrix, mat4 projMatrix, mat4 parentMatrix)
{
	if (enabled) {
		mat4 current = parentMatrix *
			MatrixFactory::translateMat3(transform.position) *
			MatrixFactory::mat3to4(transform.rotation.toRotationMatrix()) *
			MatrixFactory::scaleMat4(transform.scale);
		if (program != nullptr) {

			for (SceneNode* node : children)
			{
				node->draw(program, viewMatrix, projMatrix, current);
			}
			program->use();
			program->setUniformMat4("ProjectionMatrix", projMatrix);
			program->setUniformMat4("ViewMatrix", viewMatrix);
			if (drawable != nullptr) {
				program->setUniformMat4("ModelMatrix", current);
				drawable->draw(program);
			}
			glCullFace(GL_BACK);
			glUseProgram(0);
		}
		else
		{
			for (SceneNode* node : children)
			{
				node->draw(parentProgram, viewMatrix, projMatrix, current);
			}
			parentProgram->use();
			parentProgram->setUniformMat4("ProjectionMatrix", projMatrix);
			parentProgram->setUniformMat4("ViewMatrix", viewMatrix);
			if (drawable != nullptr) {
				parentProgram->setUniformMat4("ModelMatrix", current);
				drawable->draw(parentProgram);
			}
			glUseProgram(0);
			glCullFace(GL_BACK);
		}
	}
}

SceneNode* SceneNode::createNode(IDrawable* d)
{
	SceneNode* node = createNode();
	node->drawable = d;
	return node;
}

SceneNode* SceneNode::createNode()
{
	SceneNode* node = new SceneNode();
	node->_world = _world;
	node->enabled = true;
	node->transform.position = { 0,0,0 };
	node->transform.rotation = { 1,{0,0,0} };
	node->transform.scale = { 1,1,1 };
	children.push_back(node);
	return node;
}

void SceneNode::updateTransform(float remainingFactor)
{
	if (rigidBody) {
		rp3d::Transform currTransform = rigidBody->getTransform();
		rp3d::Transform interpolatedTransform = rp3d::Transform::interpolateTransforms(transform, currTransform, remainingFactor);
		transform = Transform::fromPhysicsTransform(interpolatedTransform, transform.scale);
		transform = Transform::fromPhysicsTransform(rigidBody->getTransform(), transform.scale);
	}
	for (SceneNode* child : children)
	{
		child->updateTransform(remainingFactor);
	}
}

void SceneNode::setTransform(Transform t)
{
	transform = t;
	rigidBody->setTransform(transform);
}

void SceneNode::setPosition(vec3 pos)
{
	transform.position = pos;
	setTransform(transform);
}

void SceneNode::setRotation(quaternion rot)
{
	transform.rotation = rot;
	setTransform(transform);
}

SceneNode* Scene::createNode(IDrawable* d)
{
	SceneNode* node = createNode();
	node->drawable = d;
	return node;
}

SceneNode* Scene::createNode()
{
	SceneNode* node = new SceneNode();
	node->enabled = true;
	node->_world = world;
	node->transform.position = { 0,0,0 };
	node->transform.rotation = { 1,{0,0,0} };
	node->transform.scale = { 1,1,1 };
	children.push_back(node);
	return node;
}

SceneNode* Scene::createNode(IDrawable* d, Transform initT, Mesh* mesh, bool isScenery)
{
	SceneNode* node = createNode(d);
	node->transform = initT;
	if (isScenery) {
		node->_vertices = new float[3 * mesh->vertices.size()];
		int curVertex = 0;
		for (vec3 v : mesh->vertices)
		{
			node->_vertices[curVertex * 3] = v.x * initT.scale.x;
			node->_vertices[curVertex * 3 + 1] = v.y * initT.scale.y;
			node->_vertices[curVertex * 3 + 2] = v.z * initT.scale.z;
			curVertex++;
		}
		node->_indices = new int[3 * mesh->faces.size()];
		int curIndex = 0;
		for (Face face : mesh->faces) {
			for (int index : face.vertices)
			{
				node->_indices[curIndex++] = index;
			}
		}
		rp3d::TriangleVertexArray* triangleArray = new rp3d::TriangleVertexArray(
			mesh->vertices.size(),
			node->_vertices,
			3 * sizeof(float),
			mesh->faces.size(),
			node->_indices, 3 * sizeof(int),
			rp3d::TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
			rp3d::TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE
		);
		node->_tMesh.addSubpart(triangleArray);
		auto shape = new rp3d::ConcaveMeshShape(&node->_tMesh);

		node->rigidBody = world->createRigidBody(node->transform);
		node->proxyShape = node->rigidBody->addCollisionShape(shape, rp3d::Transform::identity(), 1);
		node->rigidBody->setType(rp3d::BodyType::STATIC);
	}
	else {
		int numIndices = 0;
		for (Face face : mesh->faces)
		{
			numIndices += face.vertices.size();
		}
		node->_vertices = new float[mesh->vertices.size() * 3];
		node->_indices = new int[numIndices];
		auto polygonFaces = new rp3d::PolygonVertexArray::PolygonFace[mesh->faces.size()];
		int curVert = 0;
		for (vec3 vert : mesh->vertices) {
			node->_vertices[curVert * 3] = vert.x;
			node->_vertices[curVert * 3 + 1] = vert.y;
			node->_vertices[curVert * 3 + 2] = vert.z;
			curVert++;
		}
		int curIndex = 0;
		for (int i = 0; i < mesh->faces.size(); i++)
		{
			polygonFaces[i].indexBase = curIndex;
			polygonFaces[i].nbVertices = mesh->faces[i].vertices.size();
			for (int index : mesh->faces[i].vertices)
			{
				node->_indices[curIndex] = index;
				curIndex++;
			}
		}
		auto polygonVertexArray = new rp3d::PolygonVertexArray(
			mesh->vertices.size(),
			node->_vertices,
			3 * sizeof(float),
			node->_indices,
			sizeof(int),
			mesh->faces.size(),
			polygonFaces,
			rp3d::PolygonVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
			rp3d::PolygonVertexArray::IndexDataType::INDEX_INTEGER_TYPE
		);
		auto polyMesh = new rp3d::PolyhedronMesh(polygonVertexArray);
		auto shape = new rp3d::ConvexMeshShape(polyMesh, initT.scale);

		node->rigidBody = world->createRigidBody(node->transform);
		node->proxyShape = node->rigidBody->addCollisionShape(shape, rp3d::Transform::identity(), 1);
		node->rigidBody->setCenterOfMassLocal(mesh->getVertexCenter());
		//node->rigidBody->recomputeMassInformation();
	}
	return node;
}

SceneNode* SceneNode::createNode(IDrawable* d, Transform initT, Mesh* mesh, bool isScenery)
{
	SceneNode* node = createNode(d);
	node->transform = initT;
	if (isScenery) {
		node->_vertices = new float[3 * mesh->vertices.size()];
		int curVertex = 0;
		for (vec3 v : mesh->vertices)
		{
			node->_vertices[curVertex * 3] = v.x * initT.scale.x;
			node->_vertices[curVertex * 3 + 1] = v.y * initT.scale.y;
			node->_vertices[curVertex * 3 + 2] = v.z * initT.scale.z;
			curVertex++;
		}
		node->_indices = new int[3 * mesh->faces.size()];
		int curIndex = 0;
		for (Face face : mesh->faces) {
			for (int index : face.vertices)
			{
				node->_indices[curIndex++] = index;
			}
		}
		rp3d::TriangleVertexArray* triangleArray = new rp3d::TriangleVertexArray(
			mesh->vertices.size(),
			node->_vertices,
			3 * sizeof(float),
			mesh->faces.size(),
			node->_indices, 3 * sizeof(int),
			rp3d::TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
			rp3d::TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE
		);
		node->_tMesh.addSubpart(triangleArray);
		auto shape = new rp3d::ConcaveMeshShape(&node->_tMesh);

		node->rigidBody = _world->createRigidBody(node->transform);
		node->proxyShape = node->rigidBody->addCollisionShape(shape, rp3d::Transform::identity(), 1);
		node->rigidBody->setType(rp3d::BodyType::STATIC);
	}
	else {
		int numIndices = 0;
		for (Face face : mesh->faces)
		{
			numIndices += face.vertices.size();
		}
		node->_vertices = new float[mesh->vertices.size() * 3];
		node->_indices = new int[numIndices];
		auto polygonFaces = new rp3d::PolygonVertexArray::PolygonFace[mesh->faces.size()];
		int curVert = 0;
		for (vec3 vert : mesh->vertices) {
			node->_vertices[curVert * 3] = vert.x;
			node->_vertices[curVert * 3 + 1] = vert.y;
			node->_vertices[curVert * 3 + 2] = vert.z;
			curVert++;
		}
		int curIndex = 0;
		for (int i = 0; i < mesh->faces.size(); i++)
		{
			polygonFaces[i].indexBase = curIndex;
			polygonFaces[i].nbVertices = mesh->faces[i].vertices.size();
			for (int index : mesh->faces[i].vertices)
			{
				node->_indices[curIndex] = index;
				curIndex++;
			}
		}
		auto polygonVertexArray = new rp3d::PolygonVertexArray(
			mesh->vertices.size(),
			node->_vertices,
			3 * sizeof(float),
			node->_indices,
			sizeof(int),
			mesh->faces.size(),
			polygonFaces,
			rp3d::PolygonVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
			rp3d::PolygonVertexArray::IndexDataType::INDEX_INTEGER_TYPE
		);
		auto polyMesh = new rp3d::PolyhedronMesh(polygonVertexArray);
		auto shape = new rp3d::ConvexMeshShape(polyMesh, initT.scale);

		node->rigidBody = _world->createRigidBody(node->transform);
		node->proxyShape = node->rigidBody->addCollisionShape(shape, rp3d::Transform::identity(), 1);
		node->rigidBody->setCenterOfMassLocal(mesh->getVertexCenter());
		//node->rigidBody->recomputeMassInformation();
	}
	return node;
}

void Scene::physicsUpdate(float deltaTime)
{
	_timeAcum += deltaTime;

	while (_timeAcum >= physicsTimeStep) {
		world->update(physicsTimeStep);
		_timeAcum -= physicsTimeStep;
	}
	for (SceneNode* node : children) {
		node->updateTransform(_timeAcum / physicsTimeStep);
	}
}

void Scene::draw()
{
	mat4 projMatrix = cam->getProjectionMatrix();
	mat4 viewMatrix = cam->getViewMatrix();
	mainProgram->use();
	mainProgram->setUniformMat4("ProjectionMatrix", projMatrix);
	mainProgram->setUniformMat4("ViewMatrix", viewMatrix);
	for (SceneNode* node : children) {
		node->draw(mainProgram, viewMatrix, projMatrix, MatrixFactory::identity4());
	}
}

Transform Transform::fromPhysicsTransform(rp3d::Transform t, vec3 s)
{
	return { vec3::fromPhysicsVector(t.getPosition()),s,quaternion::fromPhysicsQ(t.getOrientation()) };
}

rp3d::Transform Transform::toPhysicsTransform()
{
	return rp3d::Transform(position, rotation);
}

Transform::operator rp3d::Transform()
{
	return toPhysicsTransform();
}
