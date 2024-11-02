#pragma once

#define _USE_MATH_DEFINES

#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef M_PI
	#define M_PI 3.1415926535897932384626433832795
#endif

typedef struct s_vec2 {
	float x, y;
} vec2;

typedef struct s_vec3 {
	float x, y, z;
} vec3;

typedef struct s_mat4 {
	float m[4][4];
} mat4;

typedef struct s_plane {
	vec3 point;
	vec3 normal;
} Plane;

mat4 getIdentity();
mat4 projectionMatrix(float fov, float aspectRatio, float nearPlane, float farPlane);
mat4 orthographicMatrix(float left, float right, float bottom, float top, float nearPlane, float farPlane);
mat4 viewMatrix(vec3 position, vec3 focus, vec3 up);
mat4 translationMatrix(vec3 translation);
mat4 rotationMatrix(vec3 rotation);
mat4 scaleMatrix(vec3 scale);

mat4 mat4_multiply(const mat4 *a, const mat4 *b);

vec3 transform(mat4 matrix, vec3 v);

vec3 vec3_add(vec3 a, vec3 b);
vec3 vec3_subtract(vec3 a, vec3 b);
vec3 vec3_multiply(vec3 a, vec3 b);
vec3 vec3_scale(vec3 v, float s);
vec3 vec3_lerp(vec3 a, vec3 v, float t);
vec3 vec3_normalize(vec3 v);
vec3 vec3_crossProduct(vec3 a, vec3 b);
vec3 vec3_scaleAlongVector(vec3 p, vec3 v, float s);
float dotProduct(vec3 a, vec3 b);
float length(vec3 p);

float lerp(float a, float b, float t);
float radians(float degrees);

bool intersectEdgeWithPlane(vec3 A, vec3 B, Plane plane, vec3 *intersection);
bool pointOnPlane(vec3 point, Plane plane);