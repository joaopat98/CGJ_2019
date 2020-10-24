#ifndef SHADERS_H
#define SHADERS_H

#include <GL/glew.h>
#include <string>
#include <exception>
#include <vector>
#include <unordered_map>
#include "vectors.h"
#include "matrix.h"

using namespace std;

class UniformNotFoundException : public exception {
public:
	string uniformName;
	UniformNotFoundException(string name);
};

struct Shader {
	string errorMsg;
	bool good;
	string path;
	GLuint id;
	GLuint type;
	vector<GLint> programs;
	Shader();
	Shader(string path, GLuint type);
	bool compile();
	void destroy();
};

struct Program {
	GLuint id;
	vector<Shader> shaders;
	string errorMsg;
	unordered_map<string, GLint> uniforms;
	Program();
	void create();
	bool link();
	void use();
	void destroy();
	void addShader(Shader s);
	GLint getUniformLocation(const char * name);
	void setUniform1i(const char * name, int val);
	void setUniform1f(const char * name, float val);
	void setUniform4fv(const char * name, float * val);
	void setUniform4f(const char * name, float v0, float v1, float v2, float v3);
	void setUniform3fv(const char * name, float * val);
	void setUniform3f(const char * name, float v0, float v1, float v2);
	void setUniformv2(const char * name, vec2 v);
	void setUniformv3(const char * name, vec3 v);
	void setUniformv4(const char * name, vec4 v);
	void setUniformMat2(const char * name, mat2 m);
	void setUniformMat3(const char * name, mat3 m);
	void setUniformMat4(const char * name, mat4 m);
};

#endif // !SHADERS_H
