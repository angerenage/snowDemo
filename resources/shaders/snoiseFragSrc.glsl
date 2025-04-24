#version 330 core

out vec4 fragColor;

uniform vec2 resolution;
uniform vec2 offset;

const vec2  GRAD2[8] = vec2[8](
	vec2( 1, 1), vec2(-1, 1), vec2( 1,-1), vec2(-1,-1),
	vec2( 1, 0), vec2(-1, 0), vec2( 0, 1), vec2( 0,-1)
);
const float F2 = (sqrt(3.0) - 1.0) / 2.0;
const float G2 = (3.0 - sqrt(3.0)) / 6.0;

float permute(float x) {
	return mod((34.0 * x + 1.0) * x, 289.0);
}

float dot2(vec2 g, vec2 v) {
	return g.x * v.x + g.y * v.y;
}

vec3 simplexNoiseWithDerivatives(vec2 v) {
	vec2 s = (v.x + v.y) * vec2(F2);
	vec2 i = floor(v + s);
	vec2 t = (i.x + i.y) * vec2(G2);
	vec2 X0 = i - t;
	vec2 x0 = v - X0;

	vec2 i1 = x0.x > x0.y ? vec2(1.0, 0.0) : vec2(0.0, 1.0);

	vec2 x1 = x0 - i1 + G2;
	vec2 x2 = x0 - 1.0 + 2.0 * G2;

	vec3 h = vec3(0.5 - dot(x0, x0), 0.5 - dot(x1, x1), 0.5 - dot(x2, x2));
	vec3 mask = step(vec3(0.0), h);
	h = h * mask * h;
	vec3 g = mask * 8.0;

	vec3 perm = vec3(
		permute(permute(i.x + 0.0) + i.y + 0.0),
		permute(permute(i.x + i1.x) + i.y + i1.y),
		permute(permute(i.x + 1.0) + i.y + 1.0)
	);

	vec3 grad = vec3(
		dot2(GRAD2[int(perm.x) & 7], x0),
		dot2(GRAD2[int(perm.y) & 7], x1),
		dot2(GRAD2[int(perm.z) & 7], x2)
	);

	vec3 dx = -2.0 * vec3(x0.x, x1.x, x2.x) * grad;
	vec3 dy = -2.0 * vec3(x0.y, x1.y, x2.y) * grad;

	vec2 deriv = vec2(dot(h, dx), dot(h, dy));
	float noise = dot(h, grad);

	return vec3(noise, deriv.x, deriv.y);
}

vec3 fbmWithDerivatives(vec2 v, int octaves, float persistence, float lacunarity) {
	float amplitude = 1.0;
	float frequency = 1.0;
	float total = 0.0;
	vec2 deriv = vec2(0.0);

	for (int i = 0; i < octaves; i++) {
		vec3 noiseAndDeriv = simplexNoiseWithDerivatives(v * frequency);
		total += noiseAndDeriv.x * amplitude;
		deriv += vec2(noiseAndDeriv.y, noiseAndDeriv.z) * amplitude;

		frequency *= lacunarity; 
		amplitude *= persistence;
	}

	return vec3(total, deriv.x, deriv.y);
}

void main() {
	vec2 uv = 2.0 * gl_FragCoord.xy / resolution.xy;

	int octaves = 4;
	float persistence = 0.5;
	float lacunarity = 2.0;
	vec3 fbmResult = fbmWithDerivatives(uv + offset, octaves, persistence, lacunarity);

	fragColor = vec4(fbmResult, 1.0);
}