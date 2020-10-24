#ifndef VECTORS_H

#define VECTORS_H

#include <iostream>
#include <sstream>

#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

bool is_equal(float val, float target);

struct vec2;
struct vec3;
struct vec4;

struct vec2
{
	float x, y;
	vec2();
	vec2(const vec2& v);
	vec2(float x, float y);

	vec2 operator+(vec2 v);
	vec2 operator-(vec2 v);
	vec2 operator-();
	void operator+=(vec2 v);
	void operator-=(vec2 v);
	bool operator==(vec2 v);
	vec2 operator*(float f);
	friend vec2 operator*(float f, vec2 v);
	void operator*=(float f);
	vec2 operator/(float f);
	void operator/=(float f);

	float length();
	vec2 normalize();
	float dot(vec2 v);
	void toArray(float *arr);

	string toString();
	friend ostream &operator<<(ostream &out, vec2 v);
	friend istream &operator>>(istream &in, vec2& v);

	operator vec3();
	operator vec4();
};

struct vec3
{
	float x, y, z;
	vec3();
	vec3(const vec3& v);
	vec3(float x, float y, float z);

	vec3 operator+(vec3 v);
	vec3 operator-(vec3 v);
	vec3 operator-();
	void operator+=(vec3 v);
	void operator-=(vec3 v);
	vec3 operator*(float f);
	friend vec3 operator*(float f, vec3 v);
	void operator*=(float f);
	vec3 operator/(float f);
	void operator/=(float f);
	bool operator==(vec3 v);

	float length();
	vec3 normalize();
	float dot(vec3 v);
	vec3 cross(vec3 v);
	string toString();
	void toArray(float *arr);

	friend ostream &operator<<(ostream &out, vec3 v);
	friend istream &operator>>(istream &in, vec3& v);

	operator vec2();
	operator vec4();
};

struct vec4
{
	float x, y, z, w;
	vec4();
	vec4(const vec4& v);
	vec4(float x, float y, float z, float w);

	vec4 operator+(vec4 v);
	vec4 operator-(vec4 v);
	vec4 operator-();
	void operator+=(vec4 v);
	void operator-=(vec4 v);
	vec4 operator*(float f);
	friend vec4 operator*(float f, vec4 v);
	void operator*=(float f);
	vec4 operator/(float f);
	void operator/=(float f);
	bool operator==(vec4 v);

	float dot(vec4 v);
	void toArray(float *arr);
	string toString();

	friend ostream &operator<<(ostream &out, vec4 v);
	friend istream &operator>>(istream &in, vec4& v);
	operator vec2();
	operator vec3();
};

extern vec2 AXIS2_X;
extern vec2 AXIS2_Y;
extern vec3 AXIS3_X;
extern vec3 AXIS3_Y;
extern vec3 AXIS3_Z;
extern float max_delta_comp;

float deg2rad(float degrees);

#endif