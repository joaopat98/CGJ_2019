#ifndef VECTORS_H

#define VECTORS_H

#include <reactphysics3d.h>
#include <iostream>
#include <sstream>

namespace rp3d = reactphysics3d;

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
	bool operator!=(vec2 v);

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

	static vec3 fromPhysicsVector(rp3d::Vector3 v);
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
	bool operator!=(vec3 v);

	float length();
	vec3 normalize();
	float dot(vec3 v);
	vec3 cross(vec3 v);
	string toString();
	void toArray(float *arr);
	rp3d::Vector3 toPhysicsVector();
	

	friend ostream &operator<<(ostream &out, vec3 v);
	friend istream &operator>>(istream &in, vec3& v);
	operator rp3d::Vector3();

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
	bool operator!=(vec4 v);

	float dot(vec4 v);
	void toArray(float *arr);
	string toString();

	friend ostream &operator<<(ostream &out, vec4 v);
	friend istream &operator>>(istream &in, vec4& v);
	operator vec2();
	operator vec3();
};

vec2 lerpV(float t, vec2 v1, vec2 v2);
vec3 lerpV(float t, vec3 v1, vec3 v2);
vec4 lerpV(float t, vec4 v1, vec4 v2);

#define AXIS2_X vec2{ 1,0 }
#define AXIS2_Y vec2{ 0,1 }
#define AXIS3_X vec3{ 1,0,0 }
#define AXIS3_Y vec3{ 0,1,0 }
#define AXIS3_Z vec3{ 0,0,1 }

extern float max_delta_comp;

float deg2rad(float degrees);

#endif
