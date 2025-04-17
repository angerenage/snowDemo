#include "geometry.h"

Quaternion quat_normalize(Quaternion q) {
    float magnitude = sqrtf(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
    q.w /= magnitude;
    q.x /= magnitude;
    q.y /= magnitude;
    q.z /= magnitude;
    return q;
}

Quaternion quat_lerp(Quaternion q1, Quaternion q2, float t) {
    Quaternion result;
    result.w = (1 - t) * q1.w + t * q2.w;
    result.x = (1 - t) * q1.x + t * q2.x;
    result.y = (1 - t) * q1.y + t * q2.y;
    result.z = (1 - t) * q1.z + t * q2.z;
    
    result = quat_normalize(result);
    return result;
}

mat3 mat3_identity() {
	mat3 matrix = {0};
	for (int i = 0; i < 3; i++) {
		matrix.m[i][i] = 1.0f;
	}
	return matrix;
}

mat3 mat3_quaternion(Quaternion q) {
	q = quat_normalize(q);

	mat3 matrix;
	matrix.m[0][0] = 1.0f - 2.0f * q.y * q.y - 2.0f * q.z * q.z;
	matrix.m[0][1] = 2.0f * q.x * q.y - 2.0f * q.w * q.z;
	matrix.m[0][2] = 2.0f * q.x * q.z + 2.0f * q.w * q.y;
	
	matrix.m[1][0] = 2.0f * q.x * q.y + 2.0f * q.w * q.z;
	matrix.m[1][1] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z;
	matrix.m[1][2] = 2.0f * q.y * q.z - 2.0f * q.w * q.x;

	matrix.m[2][0] = 2.0f * q.x * q.z - 2.0f * q.w * q.y;
	matrix.m[2][1] = 2.0f * q.y * q.z + 2.0f * q.w * q.x;
	matrix.m[2][2] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y;
	return matrix;
}

mat4 mat4_identity() {
	mat4 matrix = {0};
	for (int i = 0; i < 4; i++) {
		matrix.m[i][i] = 1.0f;
	}
	return matrix;
}

mat4 mat4_multiply(const mat4 *a, const mat4 *b) {
	mat4 result = {0};

	for (int i = 0; i < 4; i++) {
		__m128 row_a = _mm_loadu_ps(a->m[i]);

		for (int j = 0; j < 4; j++) {
			__m128 col_b = _mm_set_ps(b->m[3][j], b->m[2][j], b->m[1][j], b->m[0][j]);

			__m128 mul = _mm_mul_ps(row_a, col_b);

			__m128 shuf1 = _mm_shuffle_ps(mul, mul, _MM_SHUFFLE(2, 3, 0, 1));
			__m128 sums1 = _mm_add_ps(mul, shuf1);

			__m128 shuf2 = _mm_shuffle_ps(sums1, sums1, _MM_SHUFFLE(1, 0, 3, 2));
			__m128 sums2 = _mm_add_ss(sums1, shuf2);

			result.m[i][j] = _mm_cvtss_f32(sums2);
		}
	}

	return result;
}

mat4 projectionMatrix(float fov, float aspectRatio, float nearPlane, float farPlane) {
	mat4 matrix = {0};
	
	float yScale = 1.0f / tanf(fov / 2.0f);
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
	vec3 f = vec3_normalize(vec3_sub(focus, position));
	vec3 r = vec3_normalize(vec3_cross(f, up));
	vec3 u = vec3_cross(r, f);

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
	mat4 matrix = mat4_identity();
	matrix.m[3][0] = translation.x;
	matrix.m[3][1] = translation.y;
	matrix.m[3][2] = translation.z;
	return matrix;
}

mat4 rotationMatrix(vec3 rotation) {
	mat4 matrix = mat4_identity();

	float cp = cosf(rotation.x);
	float sp = sinf(rotation.x);
	float cy = cosf(rotation.y);
	float sy = sinf(rotation.y);
	float cr = cosf(rotation.z);
	float sr = sinf(rotation.z);

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
	mat4 matrix = mat4_identity();
	matrix.m[0][0] = scale.x;
	matrix.m[1][1] = scale.y;
	matrix.m[2][2] = scale.z;
	return matrix;
}

mat4 transformMatrix(vec3 translation, vec3 rotation, vec3 scale) {
	mat4 translationMat = translationMatrix(translation);
	mat4 rotationMat = rotationMatrix(rotation);
	mat4 scaleMat = scaleMatrix(scale);

	mat4 tempMat = mat4_multiply(&scaleMat, &rotationMat);
	return mat4_multiply(&tempMat, &translationMat);
}

vec3 transform(mat4 matrix, vec3 v) {
	float result[4] = {0};
	__m128 vec_sse = _mm_loadu_ps((float*)&v);

	for (int i = 0; i < 4; i++) {
		__m128 mat_row = _mm_loadu_ps(matrix.m[i]);
		__m128 mul = _mm_mul_ps(mat_row, vec_sse);

		__m128 temp = _mm_add_ps(mul, _mm_movehl_ps(mul, mul));
		__m128 sum = _mm_add_ss(temp, _mm_shuffle_ps(temp, temp, 1));

		result[i] = _mm_cvtss_f32(sum);
	}

	return (vec3){result[0], result[1], result[2]};
}

vec3 vec3_add(vec3 a, vec3 b) {
	return (vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

vec3 vec3_sub(vec3 a, vec3 b) {
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
	float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	return vec3_scale(v, 1.0f / length);
}

vec3 vec3_cross(vec3 a, vec3 b) {
	return (vec3){
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	};
}

vec3 vec3_scaleAlongVector(vec3 p, vec3 v, float s) {
	vec3 v_norm = vec3_normalize(v);
	float projection_length = vec3_dot(p, v_norm);
	vec3 projection = {projection_length * v_norm.x, projection_length * v_norm.y, projection_length * v_norm.z};
	vec3 scaled_projection = {s * projection.x, s * projection.y, s * projection.z};
	vec3 result = {p.x + (scaled_projection.x - projection.x), p.y + (scaled_projection.y - projection.y), p.z + (scaled_projection.z - projection.z)};
	return result;
}

float vec3_dot(vec3 a, vec3 b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

float vec3_length(vec3 p) {
	return sqrtf(p.x * p.x + p.y * p.y + p.z * p.z);
}

vec2 vec2_subtract(vec2 a, vec2 b) {
	return (vec2){a.x - b.x, a.y - b.y};
}

vec2 vec2_scale(vec2 v, float s) {
	return (vec2){v.x * s, v.y * s};
}

float smoothMin(float a, float b, float k) {
	float res = expf(-k * a) + expf(-k * b);
	return -logf(res) / k;
}

float lerp(float a, float b, float t) {
	return a + (t) * (b - a);
}

float radians(float degrees) {
	return degrees * ((float)M_PI / 180.0f);
}

bool intersectEdgeWithPlane(vec3 A, vec3 B, Plane plane, vec3 *intersection) {
	vec3 v1_to_plane = vec3_sub(A, plane.point);
	vec3 v2_to_plane = vec3_sub(B, plane.point);

	float dist1 = vec3_dot(v1_to_plane, plane.normal);
	float dist2 = vec3_dot(v2_to_plane, plane.normal);

	if (dist1 * dist2 < 0) {
		float t = dist1 / (dist1 - dist2);
		*intersection = vec3_lerp(A, B, t);
		return true;
	}

	return false;
}

#define EPSILON 0.0001f

bool pointOnPlane(vec3 point, Plane plane) {
	float a = vec3_dot(vec3_sub(point, plane.point), plane.normal);
	return a < EPSILON && a > -EPSILON;
}

float randomFloat(float min, float max) {
	return min + (max - min) * ((float)rand() / RAND_MAX);
}