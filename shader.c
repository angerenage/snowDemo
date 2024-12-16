#include "shader.h"

GLuint compileShader(const char *vShaderCode, const char *tcsShaderCode, const char *tesShaderCode, const char *gShaderCode, const char *fShaderCode) {
	GLuint vertex = 0, tcs = 0, tes = 0, geometry = 0, fragment = 0;
	int success;
	char infoLog[512];

	// Vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
	}

	// Tessellation Control Shader (TCS)
	if (tcsShaderCode) {
		tcs = glCreateShader(GL_TESS_CONTROL_SHADER);
		glShaderSource(tcs, 1, &tcsShaderCode, NULL);
		glCompileShader(tcs);

		glGetShaderiv(tcs, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(tcs, 512, NULL, infoLog);
			printf("ERROR::SHADER::TESS_CONTROL::COMPILATION_FAILED\n%s\n", infoLog);
		}
	}

	// Tessellation Evaluation Shader (TES)
	if (tesShaderCode) {
		tes = glCreateShader(GL_TESS_EVALUATION_SHADER);
		glShaderSource(tes, 1, &tesShaderCode, NULL);
		glCompileShader(tes);

		glGetShaderiv(tes, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(tes, 512, NULL, infoLog);
			printf("ERROR::SHADER::TESS_EVALUATION::COMPILATION_FAILED\n%s\n", infoLog);
		}
	}

	// Geometry shader
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

	// Fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
	}

	// Shader program
	GLuint ID = glCreateProgram();
	glAttachShader(ID, vertex);
	if (tcsShaderCode) glAttachShader(ID, tcs);
	if (tesShaderCode) glAttachShader(ID, tes);
	if (gShaderCode) glAttachShader(ID, geometry);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
	}

	// Clean up shaders
	glDeleteShader(vertex);
	if (tcsShaderCode) glDeleteShader(tcs);
	if (tesShaderCode) glDeleteShader(tes);
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

static const char snoiseFragSrc[] = "#version 330 core\n"
"out vec4 fragColor;"

"uniform vec2 resolution;"
"uniform vec2 offset;"

"const vec2  GRAD2[8] = vec2[8]("
	"vec2( 1, 1), vec2(-1, 1), vec2( 1,-1), vec2(-1,-1),"
	"vec2( 1, 0), vec2(-1, 0), vec2( 0, 1), vec2( 0,-1)"
");"
"const float F2 = (sqrt(3.0) - 1.0) / 2.0;"
"const float G2 = (3.0 - sqrt(3.0)) / 6.0;"

"float permute(float x) {"
	"return mod((34.0 * x + 1.0) * x, 289.0);"
"}"

"float dot2(vec2 g, vec2 v) {"
	"return g.x * v.x + g.y * v.y;"
"}"

"vec3 simplexNoiseWithDerivatives(vec2 v)"
"{"
	"vec2 s = (v.x + v.y) * vec2(F2);"
	"vec2 i = floor(v + s);"
	"vec2 t = (i.x + i.y) * vec2(G2);"
	"vec2 X0 = i - t;"
	"vec2 x0 = v - X0;"

	"vec2 i1 = x0.x > x0.y ? vec2(1.0, 0.0) : vec2(0.0, 1.0);"

	"vec2 x1 = x0 - i1 + G2;"
	"vec2 x2 = x0 - 1.0 + 2.0 * G2;"

	"vec3 h = vec3(0.5 - dot(x0, x0), 0.5 - dot(x1, x1), 0.5 - dot(x2, x2));"
	"vec3 mask = step(vec3(0.0), h);"
	"h = h * mask * h;"
	"vec3 g = mask * 8.0;"

	"vec3 perm = vec3("
		"permute(permute(i.x + 0.0) + i.y + 0.0),"
		"permute(permute(i.x + i1.x) + i.y + i1.y),"
		"permute(permute(i.x + 1.0) + i.y + 1.0)"
	");"

	"vec3 grad = vec3("
		"dot2(GRAD2[int(perm.x) & 7], x0),"
		"dot2(GRAD2[int(perm.y) & 7], x1),"
		"dot2(GRAD2[int(perm.z) & 7], x2)"
	");"

	"vec3 dx = -2.0 * vec3(x0.x, x1.x, x2.x) * grad;"
	"vec3 dy = -2.0 * vec3(x0.y, x1.y, x2.y) * grad;"

	"vec2 deriv = vec2(dot(h, dx), dot(h, dy));"
	"float noise = dot(h, grad);"

	"return vec3(noise, deriv.x, deriv.y);"
"}"

"vec3 fbmWithDerivatives(vec2 v, int octaves, float persistence, float lacunarity)"
"{"
	"float amplitude = 1.0;"
	"float frequency = 1.0;"
	"float total = 0.0;"
	"vec2 deriv = vec2(0.0);"

	"for (int i = 0; i < octaves; i++) {"
		"vec3 noiseAndDeriv = simplexNoiseWithDerivatives(v * frequency);"
		"total += noiseAndDeriv.x * amplitude;"
		"deriv += vec2(noiseAndDeriv.y, noiseAndDeriv.z) * amplitude;"

		"frequency *= lacunarity;" 
		"amplitude *= persistence;"
	"}"

	"return vec3(total, deriv.x, deriv.y);"
"}"

"void main()"
"{"
	"vec2 uv = 2.0 * gl_FragCoord.xy / resolution.xy;"

	"int octaves = 4;"
	"float persistence = 0.5;"
	"float lacunarity = 2.0;"
	"vec3 fbmResult = fbmWithDerivatives(uv + offset, octaves, persistence, lacunarity);"

	"gl_FragColor = vec4(fbmResult, 1.0);"
"}";

static const char rnoiseFragSrc[] = "#version 330 core\n"
"out vec4 c;"
"in vec2 fragPos;"
"void main()"
"{"
	"c=vec4(fract(sin(dot(fragPos,vec2(12.9898,78.233)))*43758.5453123));"
"}";

// --------------------------- ATMOSPHERE ---------------------------

// https://www.shadertoy.com/view/MstBWs
static const char atmosphereFragSrc[] = R"glsl(#version 330 core
out vec4 fragColor;

in vec2 fragPos;

uniform mat4 view;
uniform float iTime;
uniform vec2 iResolution;
uniform vec3 sunPosition;
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
	worldPosition = mat3(view) * worldPosition;

	sunVector = normalize(sunPosition);
	worldVector = normalize(worldPosition);

	vec3 lightAbsorb = vec3(0.0);

	vec3 color = calcAtmosphericScatter(lightAbsorb);
	color = calculateVolumetricClouds(color, bayer16(fragPos * iResolution), lightAbsorb);

	color = robobo1221Tonemap(color * 0.5);
	color = pow(color, vec3(1.0 / 2.2));

	fragColor = vec4(color, 1.0);
})glsl";

// --------------------------- SKY SHADERS ---------------------------

static const char skyVertSrc[] = "#version 330 core\n"
"layout(location=0) in vec3 pA;"
"out vec3 fragPos;"
"uniform mat4 projection;"
"uniform mat4 view;"
"void main()"
"{"
	"fragPos=normalize(pA);"
	"gl_Position=projection*mat4(mat3(view))*vec4(pA,1.);"
"}";

static const char skyFragSrc[] = "#version 330 core\n"
"in vec3 fragPos;"
"out vec4 fragColor;"
"uniform samplerCube skybox;"
"void main()"
"{"
	"fragColor=texture(skybox,fragPos);"
	"gl_FragDepth=1.0;"
"}";

// --------------------------- SHADOW SHADERS ---------------------------

static const char shadowFragSrc[] = "#version 330 core\n"
"void main()"
"{"
	//"gl_FragDepth = gl_FragCoord.z;"
"}";

// --------------------------- TERRAIN SHADERS ---------------------------

static const char snowVertSrc[] = "#version 430 core\n"
"layout(location=0) in vec3 pA;"

"out vec2 uv;"
"out vec2 texCoords;"
"out vec3 posWorld;"

"uniform float size;"
"uniform vec3 offset;"
"uniform vec2 characterPos;"

"void main()"
"{"
	"uv = vec2((pA.xz / size + 1.0) / 2.0);"
	"texCoords = uv + (offset.xz - characterPos) / size;"
	"posWorld = vec3(pA) + offset;"
	"gl_Position = vec4(posWorld, 1.0);"
"}";

static const char snowTCSSrc[] = "#version 430 core\n"
"layout(vertices = 3) out;"

"in vec2 uv[];"
"in vec2 texCoords[];"
"in vec3 posWorld[];"

"out vec2 tcsUV[];"
"out vec2 tcsTexCoords[];"
"out vec3 tcsPosWorld[];"

"uniform vec2 characterPos;"

"void main()"
"{"
	"tcsUV[gl_InvocationID] = uv[gl_InvocationID];"
	"tcsTexCoords[gl_InvocationID] = texCoords[gl_InvocationID];"
	"tcsPosWorld[gl_InvocationID] = posWorld[gl_InvocationID];"

	"float distance = length(characterPos - tcsPosWorld[gl_InvocationID].xz);"
	"float tessellation = mix(2.0, 6.0, smoothstep(1.0, 3.0, distance));"
	"tessellation = clamp(tessellation, 1.0, 6.0);"

	"gl_TessLevelInner[0] = tessellation;"
	"gl_TessLevelOuter[0] = 2.0;"
	"gl_TessLevelOuter[1] = 2.0;"
	"gl_TessLevelOuter[2] = 2.0;"
"}";

static const char snowTESCSrc[] = "#version 430 core\n"
"layout(triangles, equal_spacing, cw) in;"

"in vec2 tcsUV[];"
"in vec2 tcsTexCoords[];"
"in vec3 tcsPosWorld[];"

"out vec3 fragPos;"
"out vec3 fragNormal;"
"out vec4 shadowSpacePos;"
"out float footDepth;"

"uniform sampler2D noiseTex;"
"uniform sampler2D heightTex;"

"uniform mat4 projection;"
"uniform mat4 view;"
"uniform mat4 shadowProjection;"
"uniform mat4 shadowView;"

"const float heightScale = 1.5;"
"const float heightOffset = 0.5;"

"float smoothMin(float a, float b, float k) {"
    "float h = max(k - abs(a - b), 0.0) / k;"
    "return min(a, b) - h * h * h * k * (1.0 / 6.0);"
"}"

"void main()"
"{"
	"vec2 uv = gl_TessCoord.x * tcsUV[0] + gl_TessCoord.y * tcsUV[1] + gl_TessCoord.z * tcsUV[2];"
	"vec2 texCoords = gl_TessCoord.x * tcsTexCoords[0] + gl_TessCoord.y * tcsTexCoords[1] + gl_TessCoord.z * tcsTexCoords[2];"
	"vec3 pos = gl_TessCoord.x * tcsPosWorld[0] + gl_TessCoord.y * tcsPosWorld[1] + gl_TessCoord.z * tcsPosWorld[2];"

	"vec3 noise = texture(noiseTex, uv).xyz * heightScale;"
	"noise.x += heightOffset;"
	"vec3 perturbation = vec3("
		"-noise.y,"
		"1.0,"
		"-noise.z"
	");"
	"fragNormal = normalize(perturbation);"

	"float k = 0.3;"
	"float height = texture(heightTex, texCoords).x;"
	"float blendedHeight = smoothMin(height, noise.x, k);"

	"footDepth = smoothstep(0.0, k, height - blendedHeight);"
	"footDepth = clamp((noise.x - blendedHeight) * 5.0, 0.0, 1.0);"

	"pos.y += blendedHeight;"
	"fragPos = pos;"

	"vec4 worldPos = vec4(pos, 1.0);"
	"shadowSpacePos = shadowProjection * shadowView * worldPos;"
	"gl_Position = projection * view * worldPos;"
"}";

static const char snowFragSrc[] = "#version 430 core\n"
"#define M_PI 3.1415926535897932384626433832795\n"
"#define NUM_LIGHTS 11\n"

"layout(std430, binding = 0) buffer StorageBuffer {"
	"vec3 lightPositions[];"
"};"

"out vec4 fragColor;"

"in vec3 fragPos;"
"in vec3 fragNormal;"
"in vec4 shadowSpacePos;"
"in float footDepth;"

"uniform vec3 sunPos;"
"uniform vec3 viewPos;"
"uniform sampler2D shadowMap;"

"float shadowCalculation()"
"{"
	"vec3 projCoords = shadowSpacePos.xyz / shadowSpacePos.w;"
	"projCoords = projCoords * 0.5 + 0.5;"
	"if (projCoords.z >= 1.0)"
		"return 1.0;"

	"float closestDepth = texture(shadowMap, projCoords.xy).r;"
	"float currentDepth = projCoords.z;"

	"float bias = 0.0;"//max(0.001 * (1.0 - dot(fragNormal, normalize(-sunPos))), 0.0001);"
	"return currentDepth - bias > closestDepth ? 0.0 : 1.0;"
"}"

"vec3 calculate_point_lighting(float sunIntensity)"
"{"
	"vec3 color = vec3(0.0);"

	"for (int i = 0; i < NUM_LIGHTS; i++) {"
		"vec3 lightPos = lightPositions[i];"

		"vec3 lightDir = normalize(lightPos - fragPos);"

		"float diff = max(dot(fragNormal, lightDir), 0.0);"
		"float attenuation = 1.0 / (1.0 + 0.09 * length(lightPos - fragPos));"
		"vec3 lightColor = vec3(1.0, 0.5, 0.0) / NUM_LIGHTS;"

		"color += diff * lightColor * attenuation * (1.0 - sunIntensity);"
	"}"

	"return color;"
"}"

"void main()"
"{"
	"vec3 baseAmbient = vec3(0.1);"

	"vec3 lightDir = normalize(sunPos);"
	"float sunIntensity = max(dot(lightDir, vec3(0.0, 1.0, 0.0)), 0.0);"
	"vec3 ambient = baseAmbient + vec3(0.3, 0.3, 0.4) * sunIntensity * 0.7;"

	"float diff = max(dot(fragNormal, lightDir), 0.0);"

	"float shadow = shadowCalculation();"

	"vec3 sunLight = shadow * (diff * vec3(1.0, 1.0, 0.9));"
	"vec3 pointLighting = calculate_point_lighting(sunIntensity);"

	"vec3 viewDir = normalize(viewPos - fragPos);"
	"float fresnel = pow(1.0 - max(dot(viewDir, fragNormal), 0.0), 5.0);"
	"vec3 fresnelReflection = vec3(0.3, 0.5, 0.7) * fresnel * sunIntensity;"

	"vec3 finalColor = ambient + sunLight + pointLighting + fresnelReflection;"
	"finalColor *= 1.0 - footDepth;"

	"fragColor = vec4(finalColor, 1.0);"
"}";

static const char updateSnowFragSrc[] = "#version 330 core\n"
"out vec4 color;"

"in vec2 fragPos;"

"uniform sampler2D previousDepthMap;"
"uniform vec2 offset;"

"void main()"
"{"
	"vec2 uv = (fragPos + 1.0) * 0.5 + offset;"
	"gl_FragDepth = texture(previousDepthMap, uv).r;"
"}";

// --------------------------- CHARACTER SHADERS ---------------------------

static const char characterVertSrc[] = "#version 430 core\n"
"layout(location=0) in vec3 position;"
"layout(location=1) in vec3 normal;"
"layout(location=2) in uint material;"
"layout(location=3) in uint bone;"

"layout(std430, binding = 0) buffer StorageBuffer {"
	"vec3 lightPositions[];"
"};"

"uniform mat4 projection;"
"uniform mat4 view;"
"uniform mat4 model;"

"struct Bone {"
	"vec3 position;"
	"vec3 lightPosition;"
	"mat3 rotation;"
	"uint parent;"
"};"
"uniform Bone bones[11];"

"out vec3 fragPos;"
"out vec3 fragNormal;"
"flat out uint fragMaterial;"

"void main()"
"{"
	"uint boneID = bone;"
	"fragPos = bones[0].position;"

	"while (boneID != bones[boneID].parent) {"
		"fragPos += bones[bones[boneID].parent].rotation * bones[boneID].position;"
		"boneID = bones[boneID].parent;"
	"}"

	"mat3 rotation = bones[bone].rotation;"
	"lightPositions[bone] = fragPos + rotation * bones[bone].lightPosition;"
	"fragPos += rotation * position;"

	"fragMaterial = material;"
	"fragNormal = normalize(transpose(inverse(mat3(model))) * rotation * normal);"
	"gl_Position = projection * view * model * vec4(fragPos, 1.0);"
"}";

static const char characterFragSrc[] = "#version 430 core\n"
"#define NUM_LIGHTS 11\n"

"layout(std430, binding = 0) buffer StorageBuffer {"
	"vec3 lightPositions[];"
"};"

"in vec3 fragPos;"
"in vec3 fragNormal;"
"flat in uint fragMaterial;"

"out vec4 fragColor;"

"uniform vec3 sunPos;"

"vec3 calculate_point_lighting(float sunIntensity)"
"{"
	"vec3 color = vec3(0.0);"

	"for (int i = 0; i < NUM_LIGHTS; i++) {"
		"vec3 lightPos = lightPositions[i];"

		"vec3 lightDir = normalize(lightPos - fragPos);"

		"float diff = max(dot(fragNormal, lightDir), 0.0);"

		"float attenuation = 1.0 / (1.0 + 0.09 * length(lightPos - fragPos));"
		"vec3 lightColor = vec3(1.0, 0.5, 0.0) / NUM_LIGHTS;"

		"color += diff * lightColor * attenuation * (1.0 - sunIntensity);"
	"}"

	"return color;"
"}"

"void main()"
"{"
	"if (fragMaterial == 1) {"
		"fragColor = vec4(1.0, 1.0, 1.0, 1.0);"
		"return;"
	"}"

	"vec3 baseAmbient = vec3(0.05);"

	"float sunIntensity = max(dot(normalize(sunPos), vec3(0.0, 1.0, 0.0)), 0.0);"
	"vec3 ambient = baseAmbient + min(vec3(0.3, 0.3, 0.4) * sunIntensity, 1.0) * 0.05;"

	"vec3 pointLighting = calculate_point_lighting(sunIntensity);"

	"fragColor = vec4(ambient + pointLighting, 1.0);"
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
	"c=vec4(texture(tex,fragPos).xyz,1.);"
"}";


GLuint debugShader;

GLuint textShader;
GLuint snoiseShader;
GLuint rnoiseShader;

GLuint atmosphereShader;
GLuint skyShader;

GLuint snowShader;
GLuint shadowSnowShader;
GLuint updateSnowShader;

GLuint characterShader;
GLuint shadowCharacterShader;

void initShaders() {
	debugShader = compileShader(debugVertSrc, NULL, NULL, NULL, debugFragSrc);

	textShader = compileShader(textVertSrc, NULL, NULL, NULL, textFragSrc);
	snoiseShader = compileShader(postVertSrc, NULL, NULL, NULL, snoiseFragSrc);
	rnoiseShader = compileShader(postVertSrc, NULL, NULL, NULL, rnoiseFragSrc);

	snowShader = compileShader(snowVertSrc, snowTCSSrc, snowTESCSrc, NULL, snowFragSrc);
	shadowSnowShader = compileShader(snowVertSrc, NULL, NULL, NULL, shadowFragSrc);
	updateSnowShader = compileShader(postVertSrc, NULL, NULL, NULL, updateSnowFragSrc);

	atmosphereShader = compileShader(postVertSrc, NULL, NULL, NULL, atmosphereFragSrc);
	skyShader = compileShader(skyVertSrc, NULL, NULL, NULL, skyFragSrc);

	characterShader = compileShader(characterVertSrc, NULL, NULL, NULL, characterFragSrc);
	shadowCharacterShader = compileShader(characterVertSrc, NULL, NULL, NULL, shadowFragSrc);
}

void cleanupShaders() {
	glDeleteProgram(debugShader);

	glDeleteProgram(textShader);
	glDeleteProgram(snoiseShader);
	glDeleteProgram(rnoiseShader);

	glDeleteProgram(snowShader);
	glDeleteProgram(shadowSnowShader);
	glDeleteProgram(updateSnowShader);

	glDeleteProgram(atmosphereShader);
	glDeleteProgram(skyShader);

	glDeleteProgram(characterShader);
	glDeleteProgram(shadowCharacterShader);
}