#ifndef  QUATERNIONS_H
#define  QUATERNIONS_H

#include "matrix.h"

struct quaternion
{
	float t;
	vec3 v;
	static quaternion fromAxisAngle(float angle, vec3 axis);
	quaternion operator + (quaternion q);
	friend quaternion operator * (float k, quaternion q);
	quaternion operator * (float f);
	quaternion operator * (quaternion q);
	vec3 operator * (vec3 v);
	quaternion operator / (float f);
	quaternion conjugate();
	float dot(quaternion q);
	float quadrance();
	quaternion inverse();
	float magnitude();
	quaternion normalize();
	mat3 toRotationMatrix();
	void clean();

	string toString();
};

quaternion lerpQ(float t, quaternion q1, quaternion q2);

#endif
