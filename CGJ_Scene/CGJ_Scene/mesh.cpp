#include "mesh.h"

struct VertexIdInfo {
	unsigned int vertexId;
	unsigned int texcoordId;
	unsigned int normalId;
};

struct FaceIdx
{
	vector<VertexIdInfo> vertices;
};
vector <vec3> Vertices, vertexData;
vector <vec2> Texcoords, texcoordData;
vector <vec3> Normals, normalData;
vector <FaceIdx> faceData;

bool TexcoordsLoaded, NormalsLoaded;

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
	FaceIdx face;
	string token;
	if (normalData.empty() && texcoordData.empty())
	{
		sin >> token;
		while(token.size() > 0)
		{
			face.vertices.push_back({ (unsigned int)stoi(token) - 1,0,0 });
			sin >> token;
		}
		faceData.push_back(face);
	}
	else
	{
		while (getline(sin, token, '/'))
		{
			VertexIdInfo v;
			if (token.size() > 0)
				v.vertexId = stoi(token) - 1;
			getline(sin, token, '/');
			if (token.size() > 0)
				v.texcoordId = stoi(token) - 1;
			getline(sin, token, ' ');
			if (token.size() > 0)
				v.normalId = stoi(token) - 1;
			face.vertices.push_back(v);
		}
		faceData.push_back(face);
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
	bool good = ifile.bad();
	while (getline(ifile, line)) {
		stringstream sline(line);
		parseLine(sline);
	}
	TexcoordsLoaded = true;
	NormalsLoaded = true;
}

void processMeshData(Mesh *mesh)
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

void freeMeshData()
{
	vertexData.clear();
	texcoordData.clear();
	normalData.clear();
	faceData.clear();
}

const void createMesh(string& filename, Mesh *mesh)
{
	loadMeshData(filename);
	processMeshData(mesh);
	freeMeshData();
}

/////////////////////////////////////////////////////////////////////// VAOs & VBOs

void createBufferObjects(Program program, GLuint *VaoId)
{
	GLuint VboVertices, VboTexcoords, VboNormals;

	glGenVertexArrays(1, VaoId);
	glBindVertexArray(*VaoId);
	{
		glGenBuffers(1, &VboVertices);
		glBindBuffer(GL_ARRAY_BUFFER, VboVertices);
		glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(vec3), &Vertices[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(program.attribs["inPosition"]);
		glVertexAttribPointer(program.attribs["inPosition"], 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);

		if (false)
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

void destroyBufferObjects(Program program, GLuint *VaoId)
{
	glBindVertexArray(*VaoId);
	program.removeAttrib("inPosition");
	program.removeAttrib("inTexcoord");
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
	createMesh(path,&m);
	createBufferObjects(program, &m.vaoId);
	m.numTris = (int)Vertices.size();
	Vertices.clear();
	Texcoords.clear();
	Normals.clear();
	return m;
}

void Mesh::draw()
{
	glBindVertexArray(vaoId);
	glFrontFace(GL_CW);
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
