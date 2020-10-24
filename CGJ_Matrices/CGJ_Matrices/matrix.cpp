#include "matrix.h"

#pragma region mat2

#pragma region constructors

mat2::mat2() {}

mat2::mat2(float v1, float v2, float v3, float v4) {
	float vals[] = { v1,v2,v3,v4 };
	memcpy(data, vals, 4 * sizeof(float));
}

mat2::mat2(float* vals) {
	memcpy(data, vals, 4 * sizeof(float));
}

mat2::mat2(const mat2& m) {
	memcpy(data, m.data, 4 * sizeof(float));
}

#pragma endregion

#pragma region operators

mat2 mat2:: operator + (mat2 m) {
	mat2 f;
	for (int i = 0; i < 4; i++)
	{
		f.data[i] = data[i] + m.data[i];
	}
	return f;
}

mat2 mat2::operator - () {
	mat2 f;
	for (int i = 0; i < 4; i++)
	{
		f.data[i] = -data[i];
	}
	return f;
}

mat2 mat2::operator - (mat2 m) {
	mat2 f;
	for (int i = 0; i < 4; i++)
	{
		f.data[i] = data[i] - m.data[i];
	}
	return f;
}

mat2 mat2::operator * (mat2 m) {
	return mat2(
		data[0] * m.data[0] + data[1] * m.data[2], data[0] * m.data[1] + data[1] * m.data[3],
		data[2] * m.data[0] + data[3] * m.data[2], data[2] * m.data[1] + data[3] * m.data[3]
	);
}

mat2 mat2::operator * (float k) {
	mat2 f;
	for (int i = 0; i < 4; i++)
	{
		f.data[i] = data[i] * k;
	}
	return f;
}

mat2 operator * (float k, mat2 m) {
	return m * k;
}

vec2 mat2::operator * (vec2 v) {
	return vec2(
		data[0] * v.x + data[1] * v.y,
		data[2] * v.x + data[3] * v.y
	);
}

bool mat2::operator == (mat2 m) {
	for (int i = 0; i < 4; i++)
	{
		if (!is_equal(data[i], m.data[i]))
			return false;
	}
	return true;
}

#pragma endregion

#pragma region methods

mat2 mat2::transpose() {
	return mat2(
		data[0], data[2],
		data[1], data[3]
	);
}

float mat2::determinant() {
	return data[0] * data[3] - data[1] * data[2];
}

mat2 mat2::inverse() {
	float det = determinant();
	if (det == 0)
		throw NonInvertableException();
	return 1 / det * mat2(
		data[3], -data[1],
		-data[2], data[0]
	);
}

string mat2::to_string() {
	ostringstream ss;
	ss << "|\t" << data[0] << "\t" << data[1] << "\t|\n"
		<< "|\t" << data[2] << "\t" << data[3] << "\t|\n\n";
	return ss.str();
}

void mat2::clean() {
	for (int i = 0; i < 4; i++)
	{
		if (is_equal(data[i], 0))
			data[i] = 0;
	}
}

void mat2::to_GL(float *arr) {
	mat2 t = transpose();
	copy(t.data, t.data + 4, arr);
}

#pragma endregion

#pragma endregion

#pragma region mat3

#pragma region constructors

mat3::mat3() {}

mat3::mat3(float v1, float v2, float v3, float v4, float v5, float v6, float v7, float v8, float v9) {
	float vals[] = { v1,v2,v3,v4,v5,v6,v7,v8,v9 };
	memcpy(data, vals, 9 * sizeof(float));
}

mat3::mat3(float* vals) {
	memcpy(data, vals, 9 * sizeof(float));
}

mat3::mat3(const mat3& m) {
	memcpy(data, m.data, 9 * sizeof(float));
}

#pragma endregion

#pragma region operators

mat3 mat3:: operator + (mat3 m) {
	mat3 f;
	for (int i = 0; i < 9; i++)
	{
		f.data[i] = data[i] + m.data[i];
	}
	return f;
}

mat3 mat3::operator - () {
	mat3 f;
	for (int i = 0; i < 9; i++)
	{
		f.data[i] = -data[i];
	}
	return f;
}

mat3 mat3::operator - (mat3 m) {
	mat3 f;
	for (int i = 0; i < 9; i++)
	{
		f.data[i] = data[i] - m.data[i];
	}
	return f;
}
mat3 mat3::operator * (mat3 m) {
	return mat3(
		data[0] * m.data[0] + data[1] * m.data[3] + data[2] * m.data[6],
		data[0] * m.data[1] + data[1] * m.data[4] + data[2] * m.data[7],
		data[0] * m.data[2] + data[1] * m.data[5] + data[2] * m.data[8],
		data[3] * m.data[0] + data[4] * m.data[3] + data[5] * m.data[6],
		data[3] * m.data[1] + data[4] * m.data[4] + data[5] * m.data[7],
		data[3] * m.data[2] + data[4] * m.data[5] + data[5] * m.data[8],
		data[6] * m.data[0] + data[7] * m.data[3] + data[8] * m.data[6],
		data[6] * m.data[1] + data[7] * m.data[4] + data[8] * m.data[7],
		data[6] * m.data[2] + data[7] * m.data[5] + data[8] * m.data[8]
	);
}

mat3 mat3::operator * (float k) {
	mat3 f;
	for (int i = 0; i < 9; i++)
	{
		f.data[i] = data[i] * k;
	}
	return f;
}
mat3 operator * (float k, mat3 m) {
	return m * k;
}
vec3 mat3::operator * (vec3 v) {
	return vec3(
		data[0] * v.x + data[1] * v.y + data[2] * v.z,
		data[3] * v.x + data[4] * v.y + data[5] * v.z,
		data[6] * v.x + data[7] * v.y + data[8] * v.z
	);
}
bool mat3::operator == (mat3 m) {
	for (int i = 0; i < 9; i++)
	{
		if (!is_equal(data[i], m.data[i]))
			return false;
	}
	return true;
}

#pragma endregion

#pragma region methods

mat3 mat3::transpose() {
	return mat3(
		data[0], data[3], data[6],
		data[1], data[4], data[7],
		data[2], data[5], data[8]
	);
}

float mat3::determinant() {
	return data[0] * (data[4] * data[8] - data[5] * data[7])
		- data[1] * (data[3] * data[8] - data[5] * data[6])
		+ data[2] * (data[3] * data[7] - data[4] * data[6]);
}

mat3 mat3::adjoint() {
	mat3 mT = transpose();
	float *d = mT.data;
	return mat3(
		d[4] * d[8] - d[5] * d[7], -(d[3] * d[8] - d[5] * d[6]), d[3] * d[7] - d[4] * d[6],
		-(d[1] * d[8] - d[2] * d[7]), d[0] * d[8] - d[2] * d[6], -(d[0] * d[7] - d[1] * d[6]),
		d[1] * d[5] - d[2] * d[4], -(d[0] * d[5] - d[2] * d[3]), d[0] * d[4] - d[1] * d[3]
	);
}

mat3 mat3::inverse() {
	float det = determinant();
	if (det == 0)
		throw NonInvertableException();
	return 1 / det * adjoint();
}

string mat3::to_string() {
	ostringstream ss;
	ss << "|\t" << data[0] << "\t" << data[1] << "\t" << data[2] << "\t|\n"
		<< "|\t" << data[3] << "\t" << data[4] << "\t" << data[5] << "\t|\n"
		<< "|\t" << data[6] << "\t" << data[7] << "\t" << data[8] << "\t|\n\n";
	return ss.str();
}

void mat3::clean() {
	for (int i = 0; i < 9; i++)
	{
		if (is_equal(data[i], 0))
			data[i] = 0;
	}
}

void mat3::to_GL(float *arr) {
	mat3 t = transpose();
	copy(t.data, t.data + 9, arr);
}

#pragma endregion

#pragma endregion

#pragma region mat4

#pragma region constructors

mat4::mat4() {}

mat4::mat4(float v1, float v2, float v3, float v4, float v5, float v6, float v7, float v8, float v9, float v10, float v11, float v12, float v13, float v14, float v15, float v16) {
	float vals[] = { v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15,v16 };
	memcpy(data, vals, 16 * sizeof(float));
}

mat4::mat4(float* vals) {
	memcpy(data, vals, 16 * sizeof(float));
}

mat4::mat4(const mat4& m) {
	memcpy(data, m.data, 16 * sizeof(float));
}

#pragma endregion

#pragma region operators

mat4 mat4:: operator + (mat4 m) {
	mat4 f;
	for (int i = 0; i < 16; i++)
	{
		f.data[i] = data[i] + m.data[i];
	}
	return f;
}

mat4 mat4::operator - () {
	mat4 f;
	for (int i = 0; i < 16; i++)
	{
		f.data[i] = -data[i];
	}
	return f;
}

mat4 mat4::operator - (mat4 m) {
	mat4 f;
	for (int i = 0; i < 16; i++)
	{
		f.data[i] = data[i] - m.data[i];
	}
	return f;
}
mat4 mat4::operator * (mat4 m) {
	return mat4(
		data[0] * m.data[0] + data[1] * m.data[4] + data[2] * m.data[8] + data[3] * m.data[12],
		data[0] * m.data[1] + data[1] * m.data[5] + data[2] * m.data[9] + data[3] * m.data[13],
		data[0] * m.data[2] + data[1] * m.data[6] + data[2] * m.data[10] + data[3] * m.data[14],
		data[0] * m.data[3] + data[1] * m.data[7] + data[2] * m.data[12] + data[3] * m.data[15],
		data[4] * m.data[0] + data[5] * m.data[4] + data[6] * m.data[8] + data[7] * m.data[12],
		data[4] * m.data[1] + data[5] * m.data[5] + data[6] * m.data[9] + data[7] * m.data[13],
		data[4] * m.data[2] + data[5] * m.data[6] + data[6] * m.data[10] + data[7] * m.data[14],
		data[4] * m.data[3] + data[5] * m.data[7] + data[6] * m.data[12] + data[7] * m.data[15],
		data[8] * m.data[0] + data[9] * m.data[4] + data[10] * m.data[8] + data[11] * m.data[12],
		data[8] * m.data[1] + data[9] * m.data[5] + data[10] * m.data[9] + data[11] * m.data[13],
		data[8] * m.data[2] + data[9] * m.data[6] + data[10] * m.data[10] + data[11] * m.data[14],
		data[8] * m.data[3] + data[9] * m.data[7] + data[10] * m.data[12] + data[11] * m.data[15],
		data[12] * m.data[0] + data[13] * m.data[4] + data[14] * m.data[8] + data[15] * m.data[12],
		data[12] * m.data[1] + data[13] * m.data[5] + data[14] * m.data[9] + data[15] * m.data[13],
		data[12] * m.data[2] + data[13] * m.data[6] + data[14] * m.data[10] + data[15] * m.data[14],
		data[12] * m.data[3] + data[13] * m.data[7] + data[14] * m.data[12] + data[15] * m.data[15]
	);
}

mat4 mat4::operator * (float k) {
	mat4 f;
	for (int i = 0; i < 16; i++)
	{
		f.data[i] = data[i] * k;
	}
	return f;
}
mat4 operator * (float k, mat4 m) {
	return m * k;
}
vec4 mat4::operator * (vec4 v) {
	return vec4(
		data[0] * v.x + data[1] * v.y + data[2] * v.z + data[3] * v.w,
		data[4] * v.x + data[5] * v.y + data[6] * v.z + data[7] * v.w,
		data[8] * v.x + data[9] * v.y + data[10] * v.z + data[11] * v.w,
		data[12] * v.x + data[13] * v.y + data[14] * v.z + data[15] * v.w
	);
}
bool mat4::operator == (mat4 m) {
	for (int i = 0; i < 16; i++)
	{
		if (!is_equal(data[i], m.data[i]))
			return false;
	}
	return true;
}

#pragma endregion

#pragma region methods

mat4 mat4::transpose() {
	return mat4(
		data[0], data[4], data[8], data[12],
		data[1], data[5], data[9], data[13],
		data[2], data[6], data[10], data[14],
		data[3], data[7], data[11], data[15]
	);
}

float mat4::determinant() {
	float det_acum = 0;
	for (int i = 0; i < 16; i++)
	{
		float nums[9];
		int n = 0;
		for (int j = 0; j < 16; j++)
		{
			if (j / 4 != i / 4 && j % 4 != i % 4) {
				nums[n++] = data[j];
			}
		}
		det_acum += mat3(nums).determinant();
	}
	return det_acum;
}

string mat4::to_string() {
	ostringstream ss;
	ss << "|\t" << data[0] << "\t" << data[1] << "\t" << data[2] << "\t" << data[3] << "\t|\n"
		<< "|\t" << data[4] << "\t" << data[5] << "\t" << data[6] << "\t" << data[7] << "\t|\n"
		<< "|\t" << data[8] << "\t" << data[9] << "\t" << data[10] << "\t" << data[11] << "\t|\n"
		<< "|\t" << data[12] << "\t" << data[13] << "\t" << data[14] << "\t" << data[15] << "\t|\n\n";
	return ss.str();
}

void mat4::clean() {
	for (int i = 0; i < 16; i++)
	{
		if (is_equal(data[i], 0))
			data[i] = 0;
	}
}

void mat4::to_GL(float *arr) {
	mat4 t = transpose();
	copy(t.data, t.data + 16, arr);
}

#pragma endregion

#pragma endregion

#pragma region MatrixFactory

#pragma region identity

mat2 MatrixFactory::identity2() {
	return mat2(
		1, 0,
		0, 1
	);
}

mat3 MatrixFactory::identity3() {
	return mat3(
		1, 0, 0,
		0, 1, 0,
		0, 0, 1
	);
}

mat4 MatrixFactory::identity4() {
	return mat4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);
}

#pragma endregion

mat3 MatrixFactory::dualMat(vec3 m) {
	return mat3(
		0, -m.z, m.y,
		m.z, 0, -m.x,
		-m.y, m.x, 0
	);
}

#pragma region transforms

mat2 MatrixFactory::scaleMat2(float sx, float sy) {
	return mat2(
		sx, 0,
		0, sy
	);
}

mat3 MatrixFactory::scaleMat3(float sx, float sy, float sz) {
	return mat3(
		sx, 0, 0,
		0, sy, 0,
		0, 0, sz
	);
}

mat3 MatrixFactory::translateMat2(float x, float y) {
	return mat3(
		1, 0, x,
		0, 1, y,
		0, 0, 1
	);
}

mat4 MatrixFactory::translateMat3(float x, float y, float z) {
	return mat4(
		1, 0, 0, x,
		0, 1, 0, y,
		0, 0, 1, z,
		0, 0, 0, 1
	);
}

mat2 MatrixFactory::rotateMat2(float angle) {
	return mat2(
		cos(deg2rad(angle)), -sin(deg2rad(angle)),
		sin(deg2rad(angle)), cos(deg2rad(angle))
	);
}

mat3 MatrixFactory::rotateMat3(float angle, vec3 axis) {
	mat3 dual = dualMat(axis.normalize());
	return identity3() + sin(deg2rad(angle)) * dual + (1 - cos(deg2rad(angle))) * dual * dual;
}

#pragma endregion

mat2 MatrixFactory::mat3to2(mat3 m) {
	return mat2(
		m.data[0], m.data[1],
		m.data[3], m.data[4]
	);
}
mat3 MatrixFactory::mat2to3(mat2 m) {
	return mat3(
		m.data[0], m.data[1], 0,
		m.data[2], m.data[3], 0,
		0, 0, 1
	);
}
mat3 MatrixFactory::mat4to3(mat4 m) {
	return mat3(
		m.data[0], m.data[1], m.data[2],
		m.data[4], m.data[5], m.data[6],
		m.data[8], m.data[9], m.data[10]
	);
}
mat4 MatrixFactory::mat3to4(mat3 m) {
	return mat4(
		m.data[0], m.data[1], m.data[2], 0,
		m.data[3], m.data[4], m.data[5], 0,
		m.data[6], m.data[7], m.data[8], 0,
		0, 0, 0, 1
	);
}

#pragma endregion


