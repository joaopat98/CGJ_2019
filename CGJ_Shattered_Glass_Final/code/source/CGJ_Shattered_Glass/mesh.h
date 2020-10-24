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
	vector<int> vertices;
	vector<int> texCoords;
	vector<int> normals;
};

struct Mesh {
	vector<vec3> vertices;
	vector<vec2> texCoords;
	vector<vec3> normals;
	GLuint vaoId;
	int numTris;
	vector<Face> faces;
	void _parseVertex(stringstream& sin);
	void _parseTexcoord(stringstream& sin);
	void _parseNormal(stringstream& sin);
	void _parseFace(stringstream& sin);
	void _parseLine(stringstream& sin);
	void _loadMeshData(string& filename);
	const void _createMesh(string& filename);
	void _createBufferObjects(Program program);
	Mesh();
	static Mesh fromObj(Program program, string path);
	static Mesh fromFaces(Program program, vector<vec3> v, vector<vec2> t, vector<vec3> n, vector<Face> f);
	vec3 getVertexCenter();
	void draw();
	void destroy(Program program);
};

#endif // !MESH_H
