#include "vectors.h"

float max_delta_comp = 0.000001f;

bool is_equal(float val, float target) {
	return val < target + max_delta_comp && val > target - max_delta_comp;
}

float deg2rad(float degrees) {
	return (float)(degrees * (M_PI / 180.0));
}

#pragma region vec2

#pragma region constructors

vec2::vec2() {}

vec2::vec2(const vec2& v) {
	x = v.x;
	y = v.y;
}

vec2::vec2(float x, float y) {
	this->x = x;
	this->y = y;
}

#pragma endregion

#pragma region operators

vec2 vec2::operator + (vec2 v) {
	return vec2(x + v.x, y + v.y);
}

vec2 vec2::operator - (vec2 v) {
	return vec2(x - v.x, y - v.y);
}

vec2 vec2::operator - () {
	return vec2(-x, -y);
}

void vec2::operator += (vec2 v) {
	x += v.x;
	y += v.y;
}

void vec2::operator -= (vec2 v) {
	x -= v.x;
	y -= v.y;
}

vec2 vec2::operator * (float f) {
	return vec2(x * f, y * f);
}

vec2 operator * (float f, vec2 v) {
	return vec2(v.x * f, v.y * f);
}

void vec2::operator *= (float f) {
	x *= f;
	y *= f;
}

bool vec2::operator == (vec2 v) {
	return is_equal(x, v.x) && is_equal(y, v.y);
}

vec2 vec2::operator / (float f) {
	return vec2(x / f, y / f);
}

void vec2::operator /= (float f) {
	x /= f;
	y /= f;
}

#pragma endregion

#pragma region methods

float vec2::length() {
	return sqrt(x * x + y * y);
}

vec2 vec2::normalize() {
	return *this / length();
}

float vec2::dot(vec2 v) {
	return x * v.x + y * v.y;
}

void vec2::toArray(float * arr)
{
	arr[0] = x;
	arr[1] = y;
}

string vec2::toString() {
	ostringstream ss;
	ss << "(" << x << ", " << y << ")\n";
	return ss.str();
}

#pragma endregion

#pragma region i/o

ostream& operator<<(ostream& out, vec2 v) {
	return out << v.x << v.y;
}

istream& operator>>(istream& in, vec2& v) {
	return in >> v.x >> v.y;
}

#pragma endregion

#pragma region converters

vec2::operator vec3() {
	return vec3(x, y, 0);
}

vec2::operator vec4() {
	return vec4(x, y, 0, 0);
}

#pragma endregion

#pragma endregion

#pragma region vec3

#pragma region constructors

vec3::vec3() {}

vec3::vec3(const vec3& v) {
	x = v.x;
	y = v.y;
	z = v.z;
}

vec3::vec3(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

#pragma endregion

#pragma region operators

vec3 vec3::operator + (vec3 v) {
	return vec3(x + v.x, y + v.y, z + v.z);
}

vec3 vec3::operator - (vec3 v) {
	return vec3(x - v.x, y - v.y, z - v.z);
}

vec3 vec3::operator - () {
	return vec3(-x, -y, -z);
}

void vec3::operator += (vec3 v) {
	x += v.x;
	y += v.y;
	z += v.z;
}

void vec3::operator -= (vec3 v) {
	x -= v.x;
	y -= v.y;
	z -= v.z;
}

vec3 vec3::operator * (float f) {
	return vec3(x * f, y * f, z*f);
}

vec3 operator * (float f, vec3 v) {
	return vec3(v.x * f, v.y * f, v.z*f);
}

void vec3::operator *= (float f) {
	x *= f;
	y *= f;
	z *= f;
}

vec3 vec3::operator / (float f) {
	return vec3(x / f, y / f, z / f);
}

void vec3::operator /= (float f) {
	x /= f;
	y /= f;
	z /= f;
}

bool vec3::operator == (vec3 v) {
	return is_equal(x, v.x) && is_equal(y, v.y) && is_equal(z, v.z);
}

#pragma endregion

#pragma region methods

float vec3::length() {
	return sqrt(x * x + y * y + z * z);
}

vec3 vec3::normalize() {
	return *this / length();
}

float vec3::dot(vec3 v) {
	return x * v.x + y * v.y + z * v.z;
}

vec3 vec3::cross(vec3 v) {
	return vec3(
		y * v.z - z * v.y,
		z * v.x - x * v.z,
		x * v.y - y * v.x
	);
}

string vec3::toString() {
	ostringstream ss;
	ss << "(" << x << ", " << y << ", " << z << ")\n";
	return ss.str();
}

void vec3::toArray(float * arr)
{
	arr[0] = x;
	arr[1] = y;
	arr[2] = z;
}

#pragma endregion

#pragma region i/o

ostream& operator<<(ostream& out, vec3 v) {
	return out << v.x << v.y << v.z;
}

istream& operator>>(istream& in, vec3& v) {
	return in >> v.x >> v.y >> v.z;
}

#pragma endregion

#pragma region converters

vec3::operator vec2() {
	return vec2(x, y);
}

vec3::operator vec4() {
	return vec4(x, y, z, 0);
}

#pragma endregion

#pragma endregion

#pragma region vec4

#pragma region constructors

vec4::vec4()
{
	x = y = z = w = 0;
}

vec4::vec4(const vec4& v) {
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
}

vec4::vec4(float x, float y, float z, float w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

#pragma endregion

#pragma region operators

vec4 vec4::operator + (vec4 v) {
	return vec4(x + v.x, y + v.y, z + v.z, w + v.w);
}

vec4 vec4::operator - (vec4 v) {
	return vec4(x - v.x, y - v.y, z - v.z, w - v.w);
}

vec4 vec4::operator - () {
	return vec4(-x, -y, -z, -w);
}

void vec4::operator += (vec4 v) {
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
}

void vec4::operator -= (vec4 v) {
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
}

vec4 vec4::operator * (float f) {
	return vec4(x * f, y * f, z*f, w * f);
}

vec4 operator * (float f, vec4 v) {
	return vec4(v.x * f, v.y * f, v.z * f, v.w * f);
}

void vec4::operator *= (float f) {
	x *= f;
	y *= f;
	z *= f;
	w *= f;
}

vec4 vec4::operator / (float f) {
	return vec4(x / f, y / f, z / f, w / f);
}

void vec4::operator /= (float f) {
	x /= f;
	y /= f;
	z /= f;
	w /= f;
}

bool vec4::operator == (vec4 v) {
	return is_equal(x, v.x) && is_equal(y, v.y) && is_equal(z, v.z) && is_equal(w, v.w);
}

#pragma endregion

#pragma region methods

float vec4::dot(vec4 v) {
	return x * v.x + y * v.y + z * v.z + w * v.w;
}

void vec4::toArray(float * arr)
{
	arr[0] = x;
	arr[1] = y;
	arr[2] = z;
	arr[3] = w;
}

string vec4::toString() {
	ostringstream ss;
	ss << "(" << x << ", " << y << ", " << z << ", " << w << ")\n";
	return ss.str();
}

#pragma endregion

#pragma region i/o

ostream& operator<<(ostream& out, vec4 v) {
	return out << v.x << v.y << v.z << v.w;
}

istream& operator>>(istream& in, vec4& v) {
	return in >> v.x >> v.y >> v.z >> v.w;
}

vec2 lerpV(float t, vec2 v1, vec2 v2)
{
	return (v1 * (1 - t)) + (v2 * t);
}

vec3 lerpV(float t, vec3 v1, vec3 v2)
{
	return (v1 * (1 - t)) + (v2 * t);
}
vec4 lerpV(float t, vec4 v1, vec4 v2)
{
	return (v1 * (1 - t)) + (v2 * t);
}

#pragma endregion

#pragma region converters

vec4::operator vec2() {
	return vec2(x, y);
}

vec4::operator vec3() {
	return vec3(x, y, z);
}

#pragma endregion

#pragma endregion
