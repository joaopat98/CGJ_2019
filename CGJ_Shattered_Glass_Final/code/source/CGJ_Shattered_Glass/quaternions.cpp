#include  "quaternions.h"

quaternion quaternion::fromAxisAngle(float angle, vec3 axis)
{
	vec4 axisn = axis.normalize();

	quaternion q;
	float a = deg2rad(angle);
	q.t = cos(a / 2.0f);
	float s = sin(a / 2.0f);
	q.v.x = axisn.x * s;
	q.v.y = axisn.y * s;
	q.v.z = axisn.z * s;

	q.clean();
	return q.normalize();
}

quaternion quaternion::fromPhysicsQ(rp3d::Quaternion q)
{
	return {q.w,vec3::fromPhysicsVector(q.getVectorV())};
}

quaternion quaternion::operator+(quaternion q)
{
	return { t + q.t, v + q.v };
}

quaternion operator*(float k, quaternion q)
{
	return { k * q.t, k * q.v };
}

quaternion quaternion::operator*(float f)
{
	return { f * t, f * v };
}

quaternion quaternion::operator*(quaternion q)
{
	return { t*q.t - v.dot(q.v), t*q.v + q.t*v + v.cross(q.v) };
}

vec3 quaternion::operator*(vec3 v)
{
	return toRotationMatrix() * v;
}

quaternion quaternion::operator/(float f)
{
	return { t / f, v / f };
}

quaternion quaternion::conjugate()
{
	return  { t, -v };
}

float quaternion::dot(quaternion q)
{
	return t * q.t + v.dot(q.v);
}

float quaternion::quadrance()
{
	return t * t + v.dot(v);
}

quaternion quaternion::inverse()
{
	float quad = quadrance();
	if (quad == 0)
		throw NonInvertableException();
	return conjugate() / quad;
}

float quaternion::magnitude()
{
	return sqrt(quadrance());
}

quaternion quaternion::normalize()
{
	return (*this) / magnitude();
}

mat3 quaternion::toRotationMatrix()
{
	quaternion norm = normalize();
	float xx = norm.v.x * norm.v.x;
	float xy = norm.v.x * norm.v.y;
	float xz = norm.v.x * norm.v.z;
	float xt = norm.v.x * norm.t;
	float yy = norm.v.y * norm.v.y;
	float yz = norm.v.y * norm.v.z;
	float yt = norm.v.y * norm.t;
	float zz = norm.v.z * norm.v.z;
	float zt = norm.v.z * norm.t;
	return mat3(
		1.0f - 2.0f * (yy + zz), 2.0f * (xy - zt), 2.0f * (xz + yt),
		2.0f * (xy + zt), 1.0f - 2.0f * (xx + zz), 2.0f * (yz - xt),
		2.0f * (xz - yt), 2.0f * (yz + xt), 1.0f - 2.0f * (xx + yy)
	);
}

void quaternion::clean()
{
	t = is_equal(t, 0) ? 0 : t;
	v.x = is_equal(v.x, 0) ? 0 : v.x;
	v.y = is_equal(v.y, 0) ? 0 : v.y;
	v.z = is_equal(v.z, 0) ? 0 : v.z;
}

rp3d::Quaternion quaternion::toPhysicsQ()
{
	auto q = normalize();
	return rp3d::Quaternion(q.v.toPhysicsVector(),q.t);
}

string quaternion::toString()
{
	ostringstream ss;
	ss << "(" << t << ", " << v.x << ", " << v.y << ", " << v.z << ")\n";
	return ss.str();
}

quaternion::operator rp3d::Quaternion()
{
	return toPhysicsQ();
}

quaternion lerpQ(float t, quaternion q1, quaternion q2)
{
	return (1 - t) * q1 + t * q2;
}
