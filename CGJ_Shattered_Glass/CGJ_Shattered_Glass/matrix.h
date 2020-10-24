#ifndef MATRIX_H

#define MATRIX_H

#include <math.h>
#include <sstream>
#include <stdlib.h>
#include <exception>

#include "vectors.h"

class NonInvertableException : public exception {
public:
	NonInvertableException();
};

struct mat2
{
	float data[4];
	mat2();
	mat2(float v1, float v2, float v3, float v4);
	mat2(float* vals);
	mat2(const mat2& m);
	mat2 operator + (mat2 m);
	mat2 operator - ();
	mat2 operator - (mat2 m);
	mat2 operator * (mat2 m);
	mat2 operator * (float k);
	friend mat2 operator * (float k, mat2 m);
	vec2 operator * (vec2 v);
	bool operator == (mat2 m);

	mat2 transpose();
	float determinant();
	mat2 inverse();
	void clean();
	void to_GL(float * arr);

	string to_string();
};

struct mat3
{
	float data[9];
	mat3();
	mat3(float v1, float v2, float v3, float v4, float v5, float v6, float v7, float v8, float v9);
	mat3(float* vals);
	mat3(const mat3& m);
	mat3 operator + (mat3 m);
	mat3 operator - ();
	mat3 operator - (mat3 m);
	mat3 operator * (mat3 m);
	mat3 operator * (float k);
	friend mat3 operator * (float k, mat3 m);
	vec3 operator * (vec3 v);
	bool operator == (mat3 m);

	mat3 transpose();
	float determinant();
	mat3 adjoint();
	mat3 inverse();
	void clean();
	void to_GL(float *arr);

	string to_string();
};

struct mat4
{
	float data[16];
	mat4();
	mat4(float v1, float v2, float v3, float v4, float v5, float v6, float v7, float v8, float v9, float v10, float v11, float v12, float v13, float v14, float v15, float v16);
	mat4(float* vals);
	mat4(const mat4& m);
	mat4 operator + (mat4 m);
	mat4 operator - ();
	mat4 operator - (mat4 m);
	mat4 operator * (mat4 m);
	mat4 operator * (float k);
	friend mat4 operator * (float k, mat4 m);
	vec4 operator * (vec4 v);
	bool operator == (mat4 m);

	mat4 transpose();
	float determinant();
	void clean();
	void to_GL(float *arr);

	string to_string();
};

struct MatrixFactory {
	static mat2 identity2();
	static mat3 identity3();
	static mat4 identity4();
	static mat3 dualMat(vec3 m);
	static mat2 scaleMat2(float sx, float sy);
	static mat3 scaleMat3(float sx, float sy, float sz);
	static mat4 scaleMat4(float sx, float sy, float sz);
	static mat2 scaleMat2(vec2 v);
	static mat3 scaleMat3(vec3 v);
	static mat4 scaleMat4(vec3 v);
	static mat3 translateMat2(float x, float y);
	static mat4 translateMat3(float x, float y, float z);
	static mat4 translateMat3(vec3 v);
	static mat2 rotateMat2(float angle);
	static mat3 rotateMat3(float angle, vec3 axis);
	static mat4 rotateMat4(float angle, vec3 axis);

	static mat2 mat3to2(mat3 m);
	static mat3 mat2to3(mat2 m);
	static mat3 mat4to3(mat4 m);
	static mat4 mat3to4(mat3 m);
};

#endif
