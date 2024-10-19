#include "shader.h"

GLuint compileShader(const char *vShaderCode, const char *gShaderCode, const char *fShaderCode) {
	GLuint vertex, geometry, fragment;
	int success;
	char infoLog[512];

	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
	}

	// geometry shader
	if (gShaderCode) {
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		
		glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(geometry, 512, NULL, infoLog);
			printf("ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n%s\n", infoLog);
		}
	}

	// fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
	}

	GLuint ID = glCreateProgram();
	glAttachShader(ID, vertex);
	if (gShaderCode) glAttachShader(ID, geometry);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
	}

	glDeleteShader(vertex);
	if (gShaderCode) glDeleteShader(geometry);
	glDeleteShader(fragment);

	return ID;
}

GLuint compileComputeShader(const char *shaderCode) {
	GLuint compute;
	int success;
	char infoLog[512];

	// compute shader
	compute = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(compute, 1, &shaderCode, NULL);
	glCompileShader(compute);
	
	glGetShaderiv(compute, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(compute, 512, NULL, infoLog);
		printf("ERROR::SHADER::COMPUTE::COMPILATION_FAILED\n%s\n", infoLog);
	}

	GLuint ID = glCreateProgram();
	glAttachShader(ID, compute);
	glLinkProgram(ID);
	
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
	}

	glDeleteShader(compute);

	return ID;
}

// --------------------------- TEXT SHADERS ---------------------------

static const char textVertSrc[] = "#version 330 core\n"
"layout(location=0) in vec3 pA;"
"out float id;"
"uniform mat4 model;"
"uniform float aspectRatio;"
"void main()"
"{"
	"id=pA.z;"
	"vec2 f=pA.xy;"
	"f.y*=aspectRatio;"
	"gl_Position=model*vec4(f.xy,-1,1);"
"}";

static const char textFragSrc[] = "#version 330 core\n"
"out vec4 c;"
"in float id;"
"uniform float time;"
"void main()"
"{"
	"float f=fract(sin(id*.1)*1e4)*.5+id/1e2;"
	"if(f>time*7.)"
		"discard;"
	"c=vec4(1.);"
"}";

// --------------------------- NOISE SHADERS ---------------------------

static const char postVertSrc[] = "#version 330 core\n"
"layout(location=0) in vec3 pA;"
"out vec2 fragPos;"
"void main()"
"{"
	"fragPos=pA.xy;"
	"gl_Position=vec4(pA,1.);"
"}";

static const char snoiseFragSrc[] = R"glsl(#version 330 core
out vec4 fragColor;

uniform vec2 resolution;
uniform vec2 offset;

vec3 permute(vec3 x) {
    return mod(((x * 34.0) + 1.0) * x, 289.0);
}

float simplex2D(vec2 v) {
	const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
						0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
						-0.577350269189626, // -1.0 + 2.0 * C.x
						0.024390243902439); // 1.0 / 41.0

	vec2 i = floor(v + dot(v, C.yy));
	vec2 x0 = v - i + dot(i, C.xx);

	vec2 i1;
	i1.x = step(x0.y, x0.x);
	i1.y = 1.0 - i1.x;

	vec4 x12 = x0.xyxy + C.xxzz;
	x12.xy -= i1;

	i = mod(i, 289.0);
	vec3 p = permute(permute(i.y + vec3(0.0, i1.y, 1.0))
					+ i.x + vec3(0.0, i1.x, 1.0));

	vec3 m = max(0.5 - vec3(dot(x0, x0), dot(x12.xy, x12.xy), dot(x12.zw, x12.zw)), 0.0);
	m = m * m;
	m = m * m;

	vec3 x = 2.0 * fract(p * C.www) - 1.0;
	vec3 h = abs(x) - 0.5;
	vec3 ox = floor(x + 0.5);
	vec3 a0 = x - ox;

	vec3 g;
	g.x = a0.x * x0.x + h.x * x0.y;
	g.y = a0.y * x12.x + h.y * x12.y;
	g.z = a0.z * x12.z + h.z * x12.w;

	return 130.0 * dot(m, g);
}

void main() {
	vec2 uv = 2.0 * gl_FragCoord.xy / resolution.xy;
	vec2 pos = uv + offset;
	float amplitude = 1.0;
	float frequency = 1.0;
	float noise = 0.0;

	float texelSize = 1.0 / 8192.0;

	vec3 normal = vec3(0.0);

	for(int i = 0; i < 10; i++) {
		float n = simplex2D(pos * frequency);
		float vL = simplex2D((pos + vec2(-texelSize, 0.0)) * frequency);
		float vR = simplex2D((pos + vec2(texelSize, 0.0)) * frequency);
		float vD = simplex2D((pos + vec2(0.0, texelSize)) * frequency);
		float vU = simplex2D((pos + vec2(0.0, -texelSize)) * frequency);

		vec2 deriv = vec2(vR - vL, vU - vD);

		float mag = length(deriv);

		amplitude = amplitude * 1.0 / (1.0 + mag * 18.0);
		noise += amplitude * n;

		deriv *= amplitude;
		normal += vec3(-deriv, 2.0 * texelSize);

		frequency *= 2.0;
		amplitude *= 0.5;
	}

	float height = (noise + 1.0) / 2.0;
	fragColor = vec4((normalize(normal) + 1.0) / 2.0, height);
})glsl";

static const char rnoiseFragSrc[] = "#version 330 core\n"
"out vec4 c;"
"in vec2 fragPos;"
"void main()"
"{"
	"c=vec4(fract(sin(dot(fragPos,vec2(12.9898,78.233)))*43758.5453123));"
"}";

// --------------------------- TERRAIN SHADERS ---------------------------

static const char terrainVertSrc[] = "#version 430 core\n"
"layout(location=0) in vec3 pA;"
"out vec3 normal;"
"out vec4 shadowSpacePos;"
"uniform mat4 projection;"
"uniform mat4 view;"
"uniform mat4 model;"
"uniform mat4 shadowView;"
"uniform mat4 shadowProjection;"
"uniform sampler2D terrainTex;"
"uniform float size;"
"void main()"
"{"
	"vec2 uv=vec2(mat3(model)*vec3((pA.xz/size+1.)/2.,0.));"
	"normal=normalize(transpose(inverse(mat3(model)))*(texture(terrainTex,uv).xyz * 2.0 - 1.0));"
	"vec4 pos=model*vec4(pA.x,pA.y+texture(terrainTex,uv).w*5.,pA.z,1.);"
	"shadowSpacePos=shadowProjection*shadowView*pos;"
	"gl_Position=projection*view*pos;"
"}";

static const char terrainFragSrc[] = "#version 330 core\n"
"#define M_PI 3.1415926535897932384626433832795\n"
"out vec4 fragColor;"
"in vec3 normal;"
"in vec4 shadowSpacePos;"
"uniform vec3 sunPos;"
"uniform sampler2D shadowMap;"
"float shadowCalculation()"
"{"
	"vec3 projCoords = shadowSpacePos.xyz / shadowSpacePos.w;"
	"projCoords = projCoords * 0.5 + 0.5;"
	"if(projCoords.z >= 1.0)"
		"return 1.0;"
	
	"float closestDepth = texture(shadowMap, projCoords.xy).r;"
	"float currentDepth = projCoords.z;"
	"float bias = max(0.005 * (1.0 - dot(normal, normalize(sunPos * -1.0))), 0.0005);"
	"return currentDepth - bias > closestDepth ? 1.0 : 0.0;"
"}"
"void main()"
"{"
	"float shadow=0.;"
	"if (sunPos.y >= 0.)"
		"shadow=1.-shadowCalculation();"
	"shadow*=clamp(dot(normalize(sunPos),normal),0.01,1.);"
	"float ambient=clamp(sunPos.y/0.2,0.,1.)*.15;"
	"shadow=clamp((shadow+ambient)/(1.+ambient),ambient,1.);"

	"vec3 color=vec3(1.,.5,.3);"
	"fragColor=vec4(color*shadow,1.);"
"}";

// --------------------------- ATMOSPHERE ---------------------------

// https://www.shadertoy.com/view/MstBWs
static const char atmosphereFragSrc[] = R"glsl(#version 330 core
out vec4 fragColor;

in vec2 fragPos;

uniform float iTime;
uniform vec2 iResolution;
uniform vec3 sunPosition;
uniform mat4 viewMatrix;
uniform sampler2D rnoise;

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

//////////////////////////////////////////////////////////////////

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

float Get3DNoise(vec3 pos) {
	float p = floor(pos.z);
	float f = pos.z - p;
	
	const float invNoiseRes = 1.0 / 64.0;
	
	float zStretch = 17.0 * invNoiseRes;
	
	vec2 coord = pos.xy * invNoiseRes + (p * zStretch);
	
	vec2 noise = vec2(texture(rnoise, coord).x, texture(rnoise, coord + zStretch).x);
	
	return mix(noise.x, noise.y, f);
}

float getClouds(vec3 p) {
	p = vec3(p.x, length(p + vec3(0.0, earthRadius, 0.0)) - earthRadius, p.z);
	
	if (p.y < cloudMinHeight || p.y > cloudMaxHeight)
		return 0.0;
	
	float time = iTime * cloudSpeed;
	vec3 movement = vec3(time, 0.0, time);
	
	vec3 cloudCoord = (p * 0.001) + movement;
	
	float noise = Get3DNoise(cloudCoord) * 0.5;
		  noise += Get3DNoise(cloudCoord * 2.0 + movement) * 0.25;
		  noise += Get3DNoise(cloudCoord * 7.0 - movement) * 0.125;
		  noise += Get3DNoise((cloudCoord + movement) * 16.0) * 0.0625;
	
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
	#define rTOperator(x) (x / sqrt(x*x+1.0))

	float l = length(color);

	color = mix(color, color * 0.5, l / (l+1.0));
	color = rTOperator(color);

	return color;
}

void main() {
	worldPosition = normalize(vec3(fragPos, 1.0));
	worldPosition = mat3(inverse(viewMatrix)) * worldPosition;

	sunVector = normalize(sunPosition);
	worldVector = normalize(worldPosition);
	
	vec3 lightAbsorb = vec3(0.0);

	vec3 color = calcAtmosphericScatter(lightAbsorb);
	color = calculateVolumetricClouds(color, bayer16(fragPos * iResolution), lightAbsorb);
	
	color = robobo1221Tonemap(color * 0.5);
	color = pow(color, vec3(1.0 / 2.2));

	fragColor = vec4(color, 1.0);
	gl_FragDepth = 1.0;
})glsl";

// --------------------------- SHADOW SHADERS ---------------------------

static const char shadowFragSrc[] = "#version 330 core\n"
"void main()"
"{"
	//"gl_FragDepth = gl_FragCoord.z;"
"}";

// --------------------------- CHARACTER SHADERS ---------------------------

static const char characterVertSrc[] = "#version 330 core\n"
"layout(location=0) in vec3 position;"
"layout(location=1) in vec3 normal;"
"layout(location=2) in uint material;"
"layout(location=3) in uint bone;"
"uniform mat4 projection;"
"uniform mat4 view;"
"uniform mat4 model;"
"uniform mat4 bones[13];"
"out vec3 fragNormal;"
"flat out uint fragMaterial;"
"void main()"
"{"
	"vec4 pos = bones[bone] * vec4(position, 1.0);"
	"fragMaterial = material;"
	"fragNormal = normalize(mat3(transpose(inverse(bones[bone] * model))) * normal);"
	"gl_Position = projection * view * model * pos;"
"}";

static const char characterFragSrc[] = "#version 330 core\n"
"in vec3 fragNormal;"
"out vec4 fragColor;"
"flat in uint fragMaterial;"
"void main()"
"{"
	"fragColor = vec4(vec3(fragMaterial), 1.0);"
"}";

// --------------------------- DEBUG SHADERS ---------------------------

static const char debugVertSrc[] = "#version 330 core\n"
"layout(location=0) in vec3 pA;"
"out vec2 fragPos;"
"void main()"
"{"
	"fragPos=(pA.xy+1.)/2.;"
	"gl_Position=vec4(pA,1.);"
"}";

static const char debugFragSrc[] = "#version 330 core\n"
"out vec4 c;"
"in vec2 fragPos;"
"uniform sampler2D tex;"
"void main()"
"{"
	"c=vec4(vec3(texture(tex,fragPos).r),1.);"
"}";


GLuint debugShader;

GLuint textShader;
GLuint snoiseShader;
GLuint rnoiseShader;

GLuint terrainShader;

GLuint atmosphereShader;

GLuint shadowShader;

GLuint characterShader;

void initShaders() {
	debugShader = compileShader(debugVertSrc, NULL, debugFragSrc);

	textShader = compileShader(textVertSrc, NULL, textFragSrc);
	snoiseShader = compileShader(postVertSrc, NULL, snoiseFragSrc);
	rnoiseShader = compileShader(postVertSrc, NULL, rnoiseFragSrc);

	terrainShader = compileShader(terrainVertSrc, NULL, terrainFragSrc);

	atmosphereShader = compileShader(postVertSrc, NULL, atmosphereFragSrc);

	shadowShader = compileShader(terrainVertSrc, NULL, shadowFragSrc);

	characterShader = compileShader(characterVertSrc, NULL, characterFragSrc);
}

void cleanupShaders() {
	glDeleteProgram(debugShader);

	glDeleteProgram(textShader);
	glDeleteProgram(snoiseShader);
	glDeleteProgram(rnoiseShader);

	glDeleteProgram(terrainShader);

	glDeleteProgram(atmosphereShader);

	glDeleteProgram(shadowShader);
}