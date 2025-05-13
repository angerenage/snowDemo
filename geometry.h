#pragma once

#define _USE_MATH_DEFINES

#include <math.h>
#include <stdbool.h>

#ifndef M_PI
	#define M_PI 3.1415926535897932384626433832795
#endif
#ifndef M_PI_2
	#define M_PI_2 1.5707963267948966192313216916398
#endif

typedef struct s_vec2 {
	float x, y;
} vec2;

typedef struct s_vec3 {
	float x, y, z;
} vec3;

typedef struct s_vec4 {
	float x, y, z, w;
} vec4;

typedef struct s_quaternion {
	float w;
	float x;
	float y;
	float z;
} Quaternion;

typedef struct s_mat3 {
	float m[3][3];
} mat3;

typedef struct s_mat4 {
	float m[4][4];
} mat4;

typedef struct s_plane {
	vec3 point;
	vec3 normal;
} Plane;

Quaternion quat_normalize(Quaternion q);
Quaternion quat_slerp(Quaternion q1, Quaternion q2, float t);

mat3 mat3_identity();
mat3 mat3_quaternion(Quaternion q);

mat4 mat4_identity();
mat4 mat4_multiply(const mat4* const a, const mat4* const b);

mat4 projectionMatrix(float fov, float aspectRatio, float nearPlane, float farPlane);
mat4 orthographicMatrix(float left, float right, float bottom, float top, float nearPlane, float farPlane);
mat4 viewMatrix(vec3 position, vec3 focus, vec3 up);
mat4 translationMatrix(vec3 translation);
mat4 rotationMatrix(vec3 rotation);
mat4 scaleMatrix(vec3 scale);
mat4 transformMatrix(vec3 translation, vec3 rotation, vec3 scale);

vec3 transform(mat4 matrix, vec3 v);

vec3 vec3_add(vec3 a, vec3 b);
vec3 vec3_sub(vec3 a, vec3 b);
vec3 vec3_multiply(vec3 a, vec3 b);
vec3 vec3_scale(vec3 v, float s);
vec3 vec3_lerp(vec3 a, vec3 b, float t);
vec3 vec3_normalize(vec3 v);
vec3 vec3_cross(vec3 a, vec3 b);
vec3 vec3_scaleAlongVector(vec3 p, vec3 v, float s);
float vec3_dot(vec3 a, vec3 b);
float vec3_length(vec3 p);

vec2 vec2_subtract(vec2 a, vec2 b);
vec2 vec2_scale(vec2 v, float s);

float smoothMin(float a, float b, float k);
float lerp(float a, float b, float t);
float radians(float degrees);

bool intersectEdgeWithPlane(vec3 A, vec3 B, Plane plane, vec3* restrict intersection);
bool pointOnPlane(vec3 point, Plane plane);

float randomFloat(float min, float max);