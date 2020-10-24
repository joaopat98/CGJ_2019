#ifndef MESH_H
#define MESH_H

#include "matrix.h"
#include "quaternions.h"
#include "shaders.h"
#include "vectors.h"
#include <iostream>
#include <fstream>

using namespace std;

struct Face {
	vector<vec3> vertices;
	vector<vec2> texCoords;
	vector<vec3> normals;
};

struct Mesh {
	GLuint vaoId;
	int numTris;
	vector<Face> faces;
	Mesh();
	static Mesh fromObj(Program program, string path);
	void draw();
	void destroy(Program program);
};

#endif // !MESH_H
