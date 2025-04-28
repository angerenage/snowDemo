#version 330 core

in vec2 fragPos;

out vec4 fragColor;

uniform mat4 view;
uniform float time;
uniform vec2 resolution;
uniform vec3 sunPosition;
uniform vec3 moonPosition;
uniform sampler2D noiseTex;
uniform sampler2D moonTex;

const float starThreshold = 0.992;
const float moonDiskSize = 0.07;
const float moonHaloSize = 0.2;
const float moonHaloStrength = 0.07;

const float angleDayToNight = radians(10.0);
const float angleNight = radians(0.0);

const float cloudSpeed = 0.02;
const float cloudHeight = 1600.0;
const float cloudThickness = 200.0;
const float cloudDensity = 0.01;
const float fogDensity = 0.00001;

const int volumetricCloudSteps = 13;
const int volumetricLightSteps = 6;

const vec3 rayleighCoeff = vec3(0.27, 0.5, 1.0) * 1e-5;
const vec3 mieCoeff = vec3(0.5e-6);

const float sunBrightness = 3.0;
const float earthRadius = 6371000.0;

float bayer2(vec2 a){
	a = floor(a);
	return fract(dot(a, vec2(.5, a.y * .75)));
}

vec2 rsi(vec3 position, vec3 direction, float radius) {
	float PoD = dot(position, direction);
	float radiusSquared = radius * radius;

	float delta = PoD * PoD + radiusSquared - dot(position, position);
	if (delta < 0.0) return vec2(-1.0);
		delta = sqrt(delta);

	return -PoD + vec2(-delta, delta);
}

#define bayer4(a)   (bayer2( .5*(a))*.25+bayer2(a))
#define bayer8(a)   (bayer4( .5*(a))*.25+bayer2(a))
#define bayer16(a)  (bayer8( .5*(a))*.25+bayer2(a))
#define bayer32(a)  (bayer16(.5*(a))*.25+bayer2(a))
#define bayer64(a)  (bayer32(.5*(a))*.25+bayer2(a))
#define bayer128(a) (bayer64(.5*(a))*.25+bayer2(a))


const float cloudMinHeight = cloudHeight;
const float cloudMaxHeight = cloudThickness + cloudMinHeight;

const float pi = acos(-1.0);
const float rPi = 1.0 / pi;
const float hPi = pi * 0.5;
const float tau = pi * 2.0;
const float rLOG2 = 1.0 / log(2.0);

vec3 worldPosition;
vec3 worldVector;
vec3 sunVector;

#define d0(x) (abs(x) + 1e-8)
#define d02(x) (abs(x) + 1e-3)

const vec3 totalCoeff = rayleighCoeff + mieCoeff;

vec3 scatter(vec3 coeff, float depth) {
	return coeff * depth;
}

vec3 absorb(vec3 coeff, float depth) {
	return exp2(scatter(coeff, -depth));
}

float calcParticleThickness(float depth) {
	depth = depth * 2.0;
	depth = max(depth + 0.01, 0.01);
	depth = 1.0 / depth;
	
	return 100000.0 * depth;
}

float calcParticleThicknessH(float depth) {
	depth = depth * 2.0 + 0.1;
	depth = max(depth + 0.01, 0.01);
	depth = 1.0 / depth;
	
	return 100000.0 * depth;   
}

float calcParticleThicknessConst(const float depth) {
	return 100000.0 / max(depth * 2.0 - 0.01, 0.01);   
}

float rayleighPhase(float x) {
	return 0.375 * (1.0 + x*x);
}

float hgPhase(float x, float g) {
	float g2 = g*g;
	return 0.25 * ((1.0 - g2) * pow(1.0 + g2 - 2.0*g*x, -1.5));
}

float miePhaseSky(float x, float depth) {
	return hgPhase(x, exp2(-0.000003 * depth));
}

float powder(float od) {
	return 1.0 - exp2(-od * 2.0);
}

float calculateScatterIntergral(float opticalDepth, float coeff) {
	float a = -coeff * rLOG2;
	float b = -1.0 / coeff;
	float c =  1.0 / coeff;

	return exp2(a * opticalDepth) * b + c;
}

vec3 calculateScatterIntergral(float opticalDepth, vec3 coeff){
	vec3 a = -coeff * rLOG2;
	vec3 b = -1.0 / coeff;
	vec3 c =  1.0 / coeff;

	return exp2(a * opticalDepth) * b + c;
}

vec3 calcAtmosphericScatter(out vec3 absorbLight) {
	const float ln2 = log(2.0);
	
	float lDotW = dot(sunVector, worldVector);
	float lDotU = dot(sunVector, vec3(0.0, 1.0, 0.0));
	float uDotW = dot(vec3(0.0, 1.0, 0.0), worldVector);
	
	float opticalDepth = calcParticleThickness(uDotW);
	float opticalDepthLight = calcParticleThickness(lDotU);
	
	vec3 scatterView = scatter(totalCoeff, opticalDepth);
	vec3 absorbView = absorb(totalCoeff, opticalDepth);
	
	vec3 scatterLight = scatter(totalCoeff, opticalDepthLight);
	absorbLight = absorb(totalCoeff, opticalDepthLight);

	vec3 absorbSun = abs(absorbLight - absorbView) / d0((scatterLight - scatterView) * ln2);
	
	vec3 mieScatter = scatter(mieCoeff, opticalDepth) * miePhaseSky(lDotW, opticalDepth);
	vec3 rayleighScatter = scatter(rayleighCoeff, opticalDepth) * rayleighPhase(lDotW);
	
	vec3 scatterSun = mieScatter + rayleighScatter;
	
	vec3 sunSpot = smoothstep(0.9999, 0.99993, lDotW) * absorbView * sunBrightness;
	
	return (scatterSun * absorbSun + sunSpot) * sunBrightness;
}

vec3 calcAtmosphericScatterTop() {
	const float ln2 = log(2.0);
	
	float lDotU = dot(sunVector, vec3(0.0, 1.0, 0.0));
	
	float opticalDepth = calcParticleThicknessConst(1.0);
	float opticalDepthLight = calcParticleThickness(lDotU);
	
	vec3 scatterView = scatter(totalCoeff, opticalDepth);
	vec3 absorbView = absorb(totalCoeff, opticalDepth);
	
	vec3 scatterLight = scatter(totalCoeff, opticalDepthLight);
	vec3 absorbLight = absorb(totalCoeff, opticalDepthLight);
	
	vec3 absorbSun = d02(absorbLight - absorbView) / d02((scatterLight - scatterView) * ln2);
	
	vec3 mieScatter = scatter(mieCoeff, opticalDepth) * 0.25;
	vec3 rayleighScatter = scatter(rayleighCoeff, opticalDepth) * 0.375;
	
	vec3 scatterSun = mieScatter + rayleighScatter;
	
	return (scatterSun * absorbSun) * sunBrightness;
}

float get3DNoise(vec3 pos) {
	float p = floor(pos.z);
	float f = pos.z - p;
	
	const float invNoiseRes = 1.0 / 64.0;
	
	float zStretch = 17.0 * invNoiseRes;
	
	vec2 coord = pos.xy * invNoiseRes + (p * zStretch);
	
	vec2 noise = vec2(texture(noiseTex, coord).x, texture(noiseTex, coord + zStretch).x);
	
	return mix(noise.x, noise.y, f);
}

float getClouds(vec3 p) {
	p = vec3(p.x, length(p + vec3(0.0, earthRadius, 0.0)) - earthRadius, p.z);
	
	if (p.y < cloudMinHeight || p.y > cloudMaxHeight)
		return 0.0;
	
	vec3 movement = vec3(time, 0.0, time) * cloudSpeed;
	
	vec3 cloudCoord = (p * 0.001) + movement;
	
	float noise = get3DNoise(cloudCoord) * 0.5;
	noise += get3DNoise(cloudCoord * 2.0 + movement) * 0.25;
	noise += get3DNoise(cloudCoord * 7.0 - movement) * 0.125;
	noise += get3DNoise((cloudCoord + movement) * 16.0) * 0.0625;
	
	const float top = 0.004;
	const float bottom = 0.01;
	
	float horizonHeight = p.y - cloudMinHeight;
	float treshHold = (1.0 - exp2(-bottom * horizonHeight)) * exp2(-top * horizonHeight);
	
	float clouds = smoothstep(0.55, 0.6, noise);
	clouds *= treshHold;
	
	return clouds * cloudDensity;
}

float getCloudShadow(vec3 p) {
	float rSteps = cloudThickness / abs(sunVector.y);
	
	vec3 increment = sunVector * rSteps;
	vec3 position = sunVector * (cloudMinHeight - p.y) / sunVector.y + p;

	return exp2(-getClouds(position) * rSteps);
}

float getSunVisibility(vec3 p) {
	vec3 increment = sunVector * cloudThickness;
	vec3 position = increment * 0.5 + p;
	
	return exp2(-getClouds(position) * cloudThickness);
}

float phase2Lobes(float x) {
	const float m = 0.6;
	const float gm = 0.8;
	
	float lobe1 = hgPhase(x, 0.8 * gm);
	float lobe2 = hgPhase(x, -0.5 * gm);
	
	return mix(lobe2, lobe1, m);
}

vec3 getVolumetricCloudsScattering(float opticalDepth, float phase, vec3 p, vec3 sunColor, vec3 skyLight) {
	float intergal = calculateScatterIntergral(opticalDepth, 1.11);
	
	float beersPowder = powder(opticalDepth * log(2.0));
	
	vec3 sunlighting = (sunColor * getSunVisibility(p) * beersPowder) * phase * hPi * sunBrightness;
	vec3 skylighting = skyLight * 0.25 * rPi;
	
	return (sunlighting + skylighting) * intergal * pi;
}

float getHeightFogOD(float height) {
	const float falloff = 0.001;
	
	return exp2(-height * falloff) * fogDensity;
}

vec3 calculateVolumetricClouds(vec3 color, float dither, vec3 sunColor) {
	const int steps = volumetricCloudSteps;
	const float iSteps = 1.0 / float(steps);
	
	float bottomSphere = rsi(vec3(0.0, 1.0, 0.0) * earthRadius, worldVector, earthRadius + cloudMinHeight).y;
	float topSphere = rsi(vec3(0.0, 1.0, 0.0) * earthRadius, worldVector, earthRadius + cloudMaxHeight).y;
	
	vec3 startPosition = worldVector * bottomSphere;
	vec3 endPosition = worldVector * topSphere;
	
	vec3 increment = (endPosition - startPosition) * iSteps;
	vec3 cloudPosition = increment * dither + startPosition;
	
	float stepLength = length(increment);
	
	vec3 scattering = vec3(0.0);
	float transmittance = 1.0;
	
	float lDotW = dot(sunVector, worldVector);
	float phase = phase2Lobes(lDotW);
	
	vec3 skyLight = calcAtmosphericScatterTop();
	
	for (int i = 0; i < steps; i++, cloudPosition += increment){
		float opticalDepth = getClouds(cloudPosition) * stepLength;
		
		if (opticalDepth <= 0.0)
			continue;
		
		scattering += getVolumetricCloudsScattering(opticalDepth, phase, cloudPosition, sunColor, skyLight) * transmittance;
		transmittance *= exp2(-opticalDepth);
	}
	
	return mix(color * transmittance + scattering, color, clamp(length(startPosition) * 0.00001, 0.0, 1.0));
}

vec3 robobo1221Tonemap(vec3 color) {
	float l = length(color);

	color = mix(color, color * 0.5, l / (l + 1.0));
	color = color / sqrt(color * color + 1.0);

	return color;
}

float noiseStar(vec2 uv) {
	uv = fract(uv);
	float n = fract(sin(dot(uv, vec2(12.9898,78.233))) * 43758.5453);
	return n;
}

vec3 computeNightSky(vec3 viewDir) {
	vec2 uv = (viewDir.xy / max(abs(viewDir.z), 0.001)) * 20.0;
	float n = noiseStar(uv);

	float intensity = n > starThreshold ? pow((n - starThreshold) / (1.0 - starThreshold), 6.0) : 0.0;
	vec3 starColor = mix(vec3(1.0, 0.95, 0.8), vec3(0.8, 0.9, 1.0), fract(n * 10.0));
	vec3 stars = starColor * intensity;

	vec3 moonDir = normalize(moonPosition);
	float cosTheta = clamp(dot(normalize(viewDir), moonDir), -1.0, 1.0);
	float angularDistance = acos(cosTheta);
	float halo = exp(-pow(angularDistance / moonHaloSize, 2.0)) * moonHaloStrength;

	stars *= (1.0 - clamp(halo * 3.0, 0.0, 1.0) * 4.0);

	vec3 moon = vec3(0.0);
	if (angularDistance < moonDiskSize) {
		vec3 up = vec3(0.0, 1.0, 0.0);
		if (abs(dot(up, moonDir)) > 0.99) { 
			up = vec3(1.0, 0.0, 0.0);
		}
		vec3 right = normalize(cross(up, moonDir));
		vec3 moonUp = normalize(cross(moonDir, right));

		vec3 local = normalize(viewDir) - moonDir * dot(viewDir, moonDir);
		float u = dot(local, right);
		float v = dot(local, moonUp);

		vec2 moonUV = vec2(0.5) + vec2(u, v) / (moonDiskSize * 2.0);

		vec2 delta = moonUV - vec2(0.5);
		float distCenter = length(delta);

		if (distCenter < 0.41) {
			moon = texture(moonTex, moonUV).rgb;
			halo = 0.0;
		}
	}

	return stars + halo + moon;
}

void main() {
	worldPosition = normalize(vec3(fragPos, 1.0));
	worldPosition = mat3(view) * worldPosition;

	sunVector = normalize(sunPosition);
	worldVector = normalize(worldPosition);

	float sunElevation = asin(clamp(dot(sunVector, vec3(0.0, 1.0, 0.0)), -1.0, 1.0));
	float nightFactor = smoothstep(angleDayToNight, angleNight, sunElevation);

	vec3 lightAbsorb = vec3(0.0);
	vec3 dayColor = vec3(0.0);

	if (sunElevation > angleNight) {
		dayColor = calcAtmosphericScatter(lightAbsorb);
	}

	vec3 nightColor = computeNightSky(worldVector);

	vec3 color = mix(dayColor, nightColor, nightFactor);
	color = calculateVolumetricClouds(color, bayer16(fragPos * resolution), lightAbsorb);

	color = robobo1221Tonemap(color * 0.5);
	color = pow(color, vec3(1.0 / 2.2));

	fragColor = vec4(color, 1.0);
}