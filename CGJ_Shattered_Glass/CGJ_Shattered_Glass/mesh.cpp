#include "mesh.h"

void Mesh::_parseVertex(stringstream& sin)
{
	vec3 v;
	sin >> v.x >> v.y >> v.z;
	vertices.push_back(v);
}

void Mesh::_parseTexcoord(stringstream& sin)
{
	vec2 t;
	sin >> t.x >> t.y;
	texCoords.push_back(t);
}

void Mesh::_parseNormal(stringstream& sin)
{
	vec3 n;
	sin >> n.x >> n.y >> n.z;
	normals.push_back(n);
}

void Mesh::_parseFace(stringstream& sin)
{
	Face face;
	string token;
	while (getline(sin, token, '/'))
	{
		if (token.size() > 0)
			face.vertices.push_back(stoi(token) - 1);
		getline(sin, token, '/');
		if (token.size() > 0)
			face.texCoords.push_back(stoi(token) - 1);
		getline(sin, token, ' ');
		if (token.size() > 0)
			face.normals.push_back(stoi(token) - 1);
	}
	faces.push_back(face);
}

void Mesh::_parseLine(stringstream& sin)
{
	string s;
	sin >> s;
	if (s.compare("v") == 0) _parseVertex(sin);
	else if (s.compare("vt") == 0) _parseTexcoord(sin);
	else if (s.compare("vn") == 0) _parseNormal(sin);
	else if (s.compare("f") == 0) _parseFace(sin);
}

void Mesh::_loadMeshData(string& filename)
{
	ifstream ifile(filename);
	string line;
	bool good = ifile.bad();
	while (getline(ifile, line)) {
		stringstream sline(line);
		_parseLine(sline);
	}
}

/*
void processMeshData(Mesh* mesh)
{
	for (unsigned int i = 0; i < faceData.size(); i++) {
		Face face;
		for (auto v : faceData[i].vertices) {
			Vertices.push_back(vertexData[v.vertexId]);
			face.vertices.push_back(vertexData[v.vertexId]);
			if (TexcoordsLoaded) {
				Texcoords.push_back(texcoordData[v.texcoordId]);
				face.texCoords.push_back(texcoordData[v.texcoordId]);
			}
			if (NormalsLoaded) {
				Normals.push_back(normalData[v.normalId]);
				face.normals.push_back(normalData[v.normalId]);
			}
		}
		mesh->faces.push_back(face);
	}
}
*/

/*
void freeMeshData()
{
	vertexData.clear();
	texcoordData.clear();
	normalData.clear();
	faceData.clear();
}
*/

const void Mesh::_createMesh(string& filename)
{
	_loadMeshData(filename);
	//processMeshData(mesh);
	//freeMeshData();
}

/////////////////////////////////////////////////////////////////////// VAOs & VBOs

void Mesh::_createBufferObjects(Program program)
{
	GLuint VboVertices, VboTexcoords, VboNormals;
	vector<vec3> vr, nr;
	vector<vec2> tr;

	glGenVertexArrays(1, &vaoId);
	glBindVertexArray(vaoId);
	{
		for (Face face : faces) {
			for (int index : face.vertices) {
				vr.push_back(vertices[index]);
			}
			for (int index : face.texCoords) {
				tr.push_back(texCoords[index]);
			}
			for (int index : face.normals) {
				nr.push_back(normals[index]);
			}
		}

		glGenBuffers(1, &VboVertices);
		glBindBuffer(GL_ARRAY_BUFFER, VboVertices);
		glBufferData(GL_ARRAY_BUFFER, vr.size() * sizeof(vec3), &vr[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(program.attribs["inPosition"]);
		glVertexAttribPointer(program.attribs["inPosition"], 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);

		glGenBuffers(1, &VboTexcoords);
		glBindBuffer(GL_ARRAY_BUFFER, VboTexcoords);
		glBufferData(GL_ARRAY_BUFFER, tr.size() * sizeof(vec2), &tr[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(program.attribs["inTexCoord"]);
		glVertexAttribPointer(program.attribs["inTexCoord"], 2, GL_FLOAT, GL_FALSE, sizeof(vec2), 0);

		glGenBuffers(1, &VboNormals);
		glBindBuffer(GL_ARRAY_BUFFER, VboNormals);
		glBufferData(GL_ARRAY_BUFFER, nr.size() * sizeof(vec3), &nr[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(program.attribs["inNormal"]);
		glVertexAttribPointer(program.attribs["inNormal"], 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
	}
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void destroyBufferObjects(Program program, GLuint* VaoId)
{
	glBindVertexArray(*VaoId);
	program.removeAttrib("inPosition");
	program.removeAttrib("inTexCoord");
	program.removeAttrib("inNormal");
	glDeleteVertexArrays(1, VaoId);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Mesh::Mesh()
{
}

Mesh Mesh::fromObj(Program program, string path) {
	Mesh m;
	m._createMesh(path);
	m._createBufferObjects(program);
	m.numTris = m.vertices.size();
	return m;
}

Mesh Mesh::fromFaces(Program program, vector<vec3> v, vector<vec2> t, vector<vec3> n, vector<Face> f) {
	Mesh m;
	m.faces = f;
	m.vertices = v;
	m.texCoords = t;
	m.normals = n;
	m._createBufferObjects(program);
	return m;
}

vec3 Mesh::getVertexCenter()
{
	vec3 center = { 0,0,0 };
	for (vec3 v : vertices) {
		center += v;
	}
	return center / vertices.size();
}

void Mesh::draw()
{
	glBindVertexArray(vaoId);
	int faceStart = 0;
	for (Face face : faces) {
		glDrawArrays(GL_TRIANGLE_FAN, faceStart, face.vertices.size());
		faceStart += face.vertices.size();
	}
}

void Mesh::destroy(Program program)
{
	destroyBufferObjects(program, &vaoId);
}
