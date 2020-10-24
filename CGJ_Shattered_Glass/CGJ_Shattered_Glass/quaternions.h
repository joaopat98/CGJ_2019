#ifndef  QUATERNIONS_H
#define  QUATERNIONS_H

#include <reactphysics3d.h>

#include "matrix.h"

namespace rp3d = reactphysics3d;

struct quaternion
{
	float t = 0;
	vec3 v;
	static quaternion fromAxisAngle(float angle, vec3 axis);
	static quaternion fromPhysicsQ(rp3d::Quaternion q);
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
	rp3d::Quaternion toPhysicsQ();

	string toString();

	operator rp3d::Quaternion();
};

quaternion lerpQ(float t, quaternion q1, quaternion q2);

#endif
