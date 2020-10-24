#include "shaders.h"
#include <fstream>

UniformNotFoundException::UniformNotFoundException(string name)
{
	uniformName = name;
}

unsigned long getFileLength(ifstream& file)
{
	if (!file.good()) return 0;

	unsigned long pos = (unsigned long)file.tellg();
	file.seekg(0, ios::end);
	unsigned long len = (unsigned long)file.tellg();
	file.seekg(ios::beg);

	return len;
}

char* loadShader(const char* filename)
{
	ifstream file;
	file.open(filename, ios::in); // opens as ASCII!
	if (file.fail()) {
		return NULL;
	}

	int len = getFileLength(file);

	auto* ShaderSource = (char*) new char[len + 1];
	ShaderSource[len] = 0;

	unsigned int i = 0;
	while (file.good())
	{
		ShaderSource[i] = file.get();       // get character from file.
		if (!file.eof())
			i++;
	}

	ShaderSource[i] = 0;  // 0-terminate it at the correct position

	file.close();

	return ShaderSource;
}

Shader::Shader()
{
}

Shader::Shader(string path, GLuint type) {
	this->type = type;
	this->path = path;
}

bool Shader::compile() {
	auto source = loadShader(path.c_str());
	if (source == NULL) {
		errorMsg = "File \"" + path + "\" corrupted or not found\n";
	}
	id = glCreateShader(type);
	glShaderSource(id, 1, &source, 0);
	glCompileShader(id);

	GLint isCompiled = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(id, maxLength, &maxLength, &errorLog[0]);

		errorMsg = "Error compiling \"" + path + "\":\n" + string(errorLog.begin(), errorLog.end());

		glDeleteShader(id);
		return false;
	}
	return true;
}

void Shader::destroy()
{
	for (GLint prog : programs) {
		glDetachShader(prog, id);
	}
	glDeleteShader(id);
}

Program::Program()
{
}

void Program::create() {
	id = glCreateProgram();
}

bool Program::link() {
	glLinkProgram(id);
	GLint linkStatus;
	glGetProgramiv(id, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> errorLog(maxLength);
		glGetProgramInfoLog(id, maxLength, &maxLength, &errorLog[0]);

		errorMsg = string(errorLog.begin(), errorLog.end());

		return false;
	}
	GLint numActiveUniforms = 0;
	glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &numActiveUniforms);
	for (int unif = 0; unif < numActiveUniforms; ++unif)
	{
		GLsizei actualLength = 0;
		char nameArr[256];
		glGetActiveUniform(id, unif, 256, &actualLength, NULL, NULL, nameArr);
		string name(nameArr, actualLength);
		GLint location = glGetUniformLocation(id, nameArr);
		uniforms[name] = location;
	}
	return true;
}

void Program::use() {
	glUseProgram(id);
}

void Program::destroy()
{
	glUseProgram(0);
	glDeleteProgram(id);
}

void Program::addShader(Shader s)
{
	glAttachShader(id, s.id);
	s.programs.push_back(id);
}

GLint Program::getUniformLocation(const char* name) {
	try {
		return uniforms.at(name);
	}
	catch (exception e) {
		throw UniformNotFoundException(name);
	}
}

GLint Program::addAttrib(string name)
{
	GLint attrib_id = _attrib_count++;
	attribs[name] = attrib_id;
	glBindAttribLocation(id, attrib_id, name.c_str());
	return attrib_id;
}

void Program::removeAttrib(string name)
{
	if (attribs.find(name) != attribs.end()) {
		glDisableVertexAttribArray(attribs[name]);
		attribs.erase(name);
	}
}

void Program::setUniform1i(const char* name, int val) {
	glUniform1i(getUniformLocation(name), val);
}

void Program::setUniform1f(const char* name, float val) {
	glUniform1f(getUniformLocation(name), val);
}

void Program::setUniform4fv(const char* name, float* val) {
	glUniform4fv(getUniformLocation(name), 1, val);
}

void Program::setUniform4f(const char* name, float v0, float v1, float v2, float v3) {
	glUniform4f(getUniformLocation(name), v0, v1, v2, v3);
}

void Program::setUniform3fv(const char* name, float* val) {
	glUniform3fv(getUniformLocation(name), 1, val);
}

void Program::setUniform3f(const char* name, float v0, float v1, float v2) {
	glUniform3f(getUniformLocation(name), v0, v1, v2);
}

void Program::setUniformv2(const char* name, vec2 v) {
	float data[2];
	v.toArray(data);
	glUniform2fv(getUniformLocation(name), 1, data);
}

void Program::setUniformv3(const char* name, vec3 v) {
	float data[3];
	v.toArray(data);
	glUniform3fv(getUniformLocation(name), 1, data);
}

void Program::setUniformv4(const char* name, vec4 v) {
	float data[4];
	v.toArray(data);
	glUniform4fv(getUniformLocation(name), 1, data);
}

void Program::setUniformMat2(const char* name, mat2 m) {
	glUniformMatrix2fv(getUniformLocation(name), 1, GL_TRUE, m.data);
}

void Program::setUniformMat3(const char* name, mat3 m) {
	glUniformMatrix3fv(getUniformLocation(name), 1, GL_TRUE, m.data);
}

void Program::setUniformMat4(const char* name, mat4 m) {
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_TRUE, m.data);
}
