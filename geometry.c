#include "geometry.h"

mat4 getIdentity() {
	mat4 matrix = {0};
	for (int i = 0; i < 4; i++) {
		matrix.m[i][i] = 1.0f;
	}
	return matrix;
}

mat4 projectionMatrix(float fov, float aspectRatio, float nearPlane, float farPlane) {
	mat4 matrix = {0};
	
	float yScale = 1.0f / tan(fov / 2.0f);
	float xScale = yScale / aspectRatio;
	float frustumLength = farPlane - nearPlane;

	matrix.m[0][0] = xScale;
	matrix.m[1][1] = yScale;
	matrix.m[2][2] = -((farPlane + nearPlane) / frustumLength);
	matrix.m[2][3] = -1;
	matrix.m[3][2] = -((2 * nearPlane * farPlane) / frustumLength);
	
	return matrix;
}

mat4 orthographicMatrix(float left, float right, float bottom, float top, float nearPlane, float farPlane) {
	mat4 matrix = {0};
	
	matrix.m[0][0] = 2.0f / (right - left);
	matrix.m[1][1] = 2.0f / (top - bottom);
	matrix.m[2][2] = -2.0f / (farPlane - nearPlane);
	matrix.m[3][0] = -(right + left) / (right - left);
	matrix.m[3][1] = -(top + bottom) / (top - bottom);
	matrix.m[3][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
	matrix.m[3][3] = 1.0f;
	
	return matrix;
}

mat4 viewMatrix(vec3 position, vec3 focus, vec3 up) {
	vec3 f = vec3_normalize(vec3_subtract(focus, position));
	vec3 r = vec3_normalize(vec3_crossProduct(f, up));
	vec3 u = vec3_crossProduct(r, f);

	mat4 matrix = {0};
	matrix.m[0][0] = r.x;
	matrix.m[1][0] = r.y;
	matrix.m[2][0] = r.z;
	matrix.m[0][1] = u.x;
	matrix.m[1][1] = u.y;
	matrix.m[2][1] = u.z;
	matrix.m[0][2] = -f.x;
	matrix.m[1][2] = -f.y;
	matrix.m[2][2] = -f.z;
	matrix.m[3][3] = 1;

	matrix.m[3][0] = -r.x * position.x - r.y * position.y - r.z * position.z;
	matrix.m[3][1] = -u.x * position.x - u.y * position.y - u.z * position.z;
	matrix.m[3][2] = f.x * position.x + f.y * position.y + f.z * position.z;

	return matrix;
}

mat4 translationMatrix(vec3 translation) {
	mat4 matrix = getIdentity();
	matrix.m[3][0] = translation.x;
	matrix.m[3][1] = translation.y;
	matrix.m[3][2] = translation.z;
	return matrix;
}

mat4 rotationMatrix(vec3 rotation) {
	mat4 matrix = getIdentity();

	float cp = cos(rotation.x);
	float sp = sin(rotation.x);
	float cy = cos(rotation.y);
	float sy = sin(rotation.y);
	float cr = cos(rotation.z);
	float sr = sin(rotation.z);

	matrix.m[0][0] = cr * cy;
	matrix.m[0][1] = cr * sy * sp - sr * cp;
	matrix.m[0][2] = cr * sy * cp + sr * sp;
	matrix.m[1][0] = sr * cy;
	matrix.m[1][1] = sr * sy * sp + cr * cp;
	matrix.m[1][2] = sr * sy * cp - cr * sp;
	matrix.m[2][0] = -sy;
	matrix.m[2][1] = cy * sp;
	matrix.m[2][2] = cy * cp;

	return matrix;
}

mat4 scaleMatrix(vec3 scale) {
	mat4 matrix = getIdentity();
	matrix.m[0][0] = scale.x;
	matrix.m[1][1] = scale.y;
	matrix.m[2][2] = scale.z;
	return matrix;
}

mat4 mat4_multiply(const mat4 *a, const mat4 *b) {
	mat4 result = {0};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				result.m[i][j] += a->m[i][k] * b->m[k][j];
			}
		}
	}
	return result;
}

vec3 transform(mat4 matrix, vec3 v) {
	float x = matrix.m[0][0] * v.x + matrix.m[0][1] * v.y + matrix.m[0][2] * v.z + matrix.m[0][3];
	float y = matrix.m[1][0] * v.x + matrix.m[1][1] * v.y + matrix.m[1][2] * v.z + matrix.m[1][3];
	float z = matrix.m[2][0] * v.x + matrix.m[2][1] * v.y + matrix.m[2][2] * v.z + matrix.m[2][3];
	float w = matrix.m[3][0] * v.x + matrix.m[3][1] * v.y + matrix.m[3][2] * v.z + matrix.m[3][3];

	if (w != 0.0f) {
		x /= w;
		y /= w;
		z /= w;
	}

	return (vec3){x, y, z};
}

vec3 vec3_add(vec3 a, vec3 b) {
	return (vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

vec3 vec3_subtract(vec3 a, vec3 b) {
	return (vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

vec3 vec3_multiply(vec3 a, vec3 b) {
	return (vec3){a.x * b.x, a.y * b.y, a.z * b.z};
}

vec3 vec3_scale(vec3 v, float s) {
	return (vec3){v.x * s, v.y * s, v.z * s};
}

vec3 vec3_lerp(vec3 a, vec3 b, float t) {
	return vec3_add(vec3_scale(a, 1 - t), vec3_scale(b, t));
}

vec3 vec3_normalize(vec3 v) {
	float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	return vec3_scale(v, 1.0f / length);
}

vec3 vec3_crossProduct(vec3 a, vec3 b) {
	return (vec3){
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	};
}

vec3 vec3_scaleAlongVector(vec3 p, vec3 v, float s) {
	vec3 v_norm = vec3_normalize(v);
	float projection_length = dotProduct(p, v_norm);
	vec3 projection = {projection_length * v_norm.x, projection_length * v_norm.y, projection_length * v_norm.z};
	vec3 scaled_projection = {s * projection.x, s * projection.y, s * projection.z};
	vec3 result = {p.x + (scaled_projection.x - projection.x), p.y + (scaled_projection.y - projection.y), p.z + (scaled_projection.z - projection.z)};
	return result;
}

float dotProduct(vec3 a, vec3 b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

float length(vec3 p) {
	return sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
}

float lerp(float a, float b, float t) {
	return a + (t) * (b - a);
}

float radians(float degrees) {
	return degrees * (M_PI / 180);
}

bool intersectEdgeWithPlane(vec3 A, vec3 B, Plane plane, vec3 *intersection) {
	vec3 v1_to_plane = vec3_subtract(A, plane.point);
	vec3 v2_to_plane = vec3_subtract(B, plane.point);

	float dist1 = dotProduct(v1_to_plane, plane.normal);
	float dist2 = dotProduct(v2_to_plane, plane.normal);

	if (dist1 * dist2 < 0) {
		float t = dist1 / (dist1 - dist2);
		*intersection = vec3_lerp(A, B, t);
		return true;
	}

	return false;
}

#define EPSILON 0.0001f

bool pointOnPlane(vec3 point, Plane plane) {
	float a = dotProduct(vec3_subtract(point, plane.point), plane.normal);
	return a < EPSILON && a > -EPSILON;
}