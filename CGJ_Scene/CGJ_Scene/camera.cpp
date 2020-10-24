#include "camera.h"

OrthoProjection::OrthoProjection(float l, float r, float b, float t, float n, float f)
{
	left = l;
	right = r;
	bottom = b;
	top = t;
	near = n;
	far = f;
}

mat4 OrthoProjection::getProjectionMatrix() {
	return mat4(
		2 / (right - left), 0, 0, -(right + left) / (right - left),
		0, 2 / (top - bottom), 0, -(top + bottom) / (top - bottom),
		0, 0, -2 / (far - near), -(far + near) / (far - near),
		0, 0, 0, 1
	);
}

PerspectiveProjection::PerspectiveProjection(float fovy, float aspect, float nearZ, float farZ)
{
	this->fovy = fovy;
	this->aspect = aspect;
	this->nearZ = nearZ;
	this->farZ = farZ;
}

mat4 PerspectiveProjection::getProjectionMatrix() {
	return mat4(
		1 / (aspect * tan(fovy / 2)), 0, 0, 0,
		0, 1 / tan(fovy / 2), 0, 0,
		0, 0, -(nearZ + farZ) / (nearZ - farZ), (2 * nearZ * farZ) / (nearZ - farZ),
		0, 0, 1, 0
	);
}

Camera::Camera()
{
}

Camera::Camera(float d, quaternion rot, Projection *p)
{
	distance = d;
	rotation = rot;
	projection = p;
}

mat4 Camera::getProjectionMatrix() {
	return projection->getProjectionMatrix();
}

mat4 Camera::getViewMatrix() {
	return MatrixFactory::translateMat3(0, 0, distance) * MatrixFactory::mat3to4(rotation.toRotationMatrix());
}
