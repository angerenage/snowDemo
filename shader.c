#include "shader.h"

static GLuint compileShader(const char *vShaderCode, const char *tcsShaderCode, const char *tesShaderCode, const char *gShaderCode, const char *fShaderCode) {
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

static GLuint compileComputeShader(const char *shaderCode) {
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

// --------------------------- DEBUG SHADERS ---------------------------

static const char debugVertSrc[] = "#version 330 core\n"
"layout(location=0) in vec3 pA;"
"uniform mat4 model;"
"uniform mat4 view;"
"uniform mat4 projection;"
"out vec2 fragPos;"
"void main()"
"{"
	"fragPos=(pA.xy+1.)/2.;"
	"gl_Position=projection*view*model*vec4(pA,1.);"
"}";

static const char debugFragSrc[] = "#version 330 core\n"
"out vec4 c;"
"in vec2 fragPos;"
"uniform sampler2D tex;"
"void main()"
"{"
	"c=vec4(texture(tex,fragPos).xyz,1.);"
"}";

// --------------------------- BASIC SHADERS ---------------------------

static const char basicVertSrc[] = "#version 330 core\n"
"layout(location=0) in vec3 pA;"
"uniform mat4 model;"
"uniform mat4 view;"
"uniform mat4 projection;"
"void main()"
"{"
	"gl_Position=projection*view*model*vec4(pA,1.);"
"}";

static const char basicFragSrc[] = "#version 330 core\n"
"void main()"
"{"
	//"gl_FragDepth = gl_FragCoord.z;"
"}";

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
static const char atmosphereFragSrc[] = "#version 330 core\n"
"out vec4 fragColor;"
"in vec2 fragPos;"
"uniform mat4 view;"
"uniform float iTime;"
"uniform vec2 iResolution;"
"uniform vec3 sunPosition;"
"uniform sampler2D rnoise;"
"const vec3 v=vec3(.27,.5,1)*1e-5,a=vec3(5e-7);"
"vec2 e(vec3 a,float v)"
"{"
	"vec3 x=vec3(0,1,0)*6.371e6;"
	"float d=dot(x,a);"
	"v=d*d+v*v-dot(x,x);"
	"if(v<0.)"
		"return vec2(-1);"
	"v=sqrt(v);"
	"return-d+vec2(-v,v);"
"}"
"float bayer2(vec2 a)"
"{"
	"a=floor(a);"
	"return fract(dot(a,vec2(.5,a.y*.75)));"
"}\n"
"#define bayer4(a)(bayer2(.5*(a))*.25+bayer2(a))\n"
"#define bayer8(a)(bayer4(.5*(a))*.25+bayer2(a))\n"
"#define bayer16(a)(bayer8(.5*(a))*.25+bayer2(a))\n"
"#define bayer32(a)(bayer16(.5*(a))*.25+bayer2(a))\n"
"#define bayer64(a)(bayer32(.5*(a))*.25+bayer2(a))\n"
"#define bayer128(a)(bayer64(.5*(a))*.25+bayer2(a))\n"
"const float d=acos(-1.),f=1./d,l=d*.5,b=d*2.,s=1./log(2.);"
"vec3 r,x,i;\n"
"#define d0(x)(abs(x)+1e-8)\n"
"#define d02(x)(abs(x)+1e-3)\n"
"const vec3 m=v+a;"
"vec3 t(vec3 v,float a)"
"{"
	"return v*a;"
"}"
"vec3 n(vec3 v,float a)"
"{"
	"return exp2(t(v,-a));"
"}"
"float e(float a)"
"{"
	"a=1./max(a*2.+.01,.01);"
	"return 1e5*a;"
"}"
"float n(float a)"
"{"
	"return.375*(1.+a*a);"
"}"
"float g(float v,float a)"
"{"
	"float b=a*a;"
	"return(1.-b)*pow(1.+b-2.*a*v,-1.5)*.25;"
"}"
"float h(float a,float v)"
"{"
	"return exp2(-v*s*a)*(-1./v)+1./v;"
"}"
"vec3 h(float a,vec3 v)"
"{"
	"return exp2(-v*s*a)*(-1./v)+1./v;"
"}"
"vec3 g(out vec3 f)"
"{"
	"float b=dot(i,x),r=dot(i,vec3(0,1,0)),l=dot(vec3(0,1,0),x);"
	"l=e(l);"
	"r=e(r);"
	"vec3 d=n(m,l);"
	"f=n(m,r);"
	"vec3 s=abs(f-d)/d0((t(m,r)-t(m,l))*log(2.));"
	"return((t(a,l)*g(b,exp2(-3e-6*l))+t(v,l)*n(b))*s+smoothstep(.9999,.99993,b)*d*3.)*3.;"
"}"
"vec3 e()"
"{"
	"float f=dot(i,vec3(0,1,0)),l=1e5/max(1.99,.01);"
	"f=e(f);"
	"vec3 d=d02(n(m,f)-n(m,l))/d02((t(m,f)-t(m,l))*log(2.));"
	"return(t(a,l)*.25+t(v,l)*.375)*d*3.;"
"}"
"float h(vec3 a)"
"{"
	"float v=floor(a.z);"
	"const float d=1./64.;"
	"float b=17.*d;"
	"vec2 f=a.xy*d+v*b;"
	"f=vec2(texture(rnoise,f).x,texture(rnoise,f+b));"
	"return mix(f.x,f.y,a.z-v);"
"}"
"float t(vec3 a)"
"{"
	"a=vec3(a.x,length(a+vec3(0,6371000,0))-6.371e6,a.z);"
	"if(a.y<1600.||a.y>1800.)"
		"return 0.;"
	"float v=iTime*.02;"
	"vec3 b=vec3(v,0,v),f=a*.001+b;"
	"v=h(f)*.5+h(f*2.+b)*.25+h(f*7.-b)*.125+h((f+b)*16.)*.0625;"
	"float d=a.y-1600.;"
	"v=(1.-exp2(-.01*d))*exp2(-.004*d)*smoothstep(.55,.6,v);"
	"return v*.01;"
"}"
"float p(vec3 a)"
"{"
	"vec3 v=i*2e2;"
	"return exp2(-t(v*.5+a)*2e2);"
"}"
"float w(float a)"
"{"
	"return mix(g(a,-.4),g(a,.64),.6);"
"}"
"vec3 e(float v,float a,vec3 x,vec3 m,vec3 b)"
"{"
	"float s=h(v,1.11);"
	"return(m*p(x)*(1.-exp2(-v*log(2.)*2.))*a*l*3.+b*.25*f)*s*d;"
"}"
"vec3 e(vec3 v,float f,vec3 a)"
"{"
	"float d=e(x,6372600.).y,l=e(x,6372800.).y;"
	"vec3 b=x*d,r=x*l;"
	"r=(r-b)*(1./float(13));"
	"vec3 m=r*f+b;"
	"d=length(r);"
	"vec3 s=vec3(0);"
	"l=1.;"
	"f=w(dot(i,x));"
	"vec3 c=e();"
	"for(int v=0;v<13;v++,m+=r)"
		"{"
			"float b=t(m)*d;"
			"if(b<=0.)"
				"continue;"
			"s+=e(b,f,m,a,c)*l;"
			"l*=exp2(-b);"
		"}"
	"return mix(v*l+s,v,clamp(length(b)*1e-5,0.,1.));"
"}"
"vec3 c(vec3 v)"
"{"
	"\n#define rTOperator(x)(x/sqrt(x*x+1.0))\n"
	"float a=length(v);"
	"v=mix(v,v*.5,a/(a+1.));"
	"return rTOperator(v);"
"}"
"void main()"
"{"
	"r=normalize(vec3(fragPos,1));"
	"r=mat3(view)*r;"
	"i=normalize(sunPosition);"
	"x=normalize(r);"
	"vec3 a=vec3(0),v=g(a);"
	"v=e(v,bayer16(fragPos*iResolution),a);"
	"v=c(v*.5);"
	"v=pow(v,vec3(1./2.2));"
	"fragColor=vec4(v,1);"
"}";

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

// --------------------------- TERRAIN SHADERS ---------------------------

static const char snowVertSrc[] = "#version 430 core\n"
"layout(location=0) in vec3 pA;"

"out vec2 uv;"
"out vec2 texCoords;"
"out vec3 posWorld;"

"uniform mat4 model;"
"uniform float size;"
"uniform vec3 offset;"
"uniform vec2 characterPos;"

"void main()"
"{"
	"vec4 pos = model * vec4(pA, 1.0);"
	"uv = vec2((pos.xz / size + 1.0) / 2.0);"
	"posWorld = pos.xyz + offset;"
	"texCoords = ((posWorld.xz - characterPos) / size + 1.0) / 2.0;"
	"gl_Position = pos;"
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

"out vec2 tesUV;"
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

	"tesUV = texCoords;"

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

"in vec2 tesUV;"

"uniform vec3 lightPos;"
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

	"float bias = 0.0;"//max(0.001 * (1.0 - dot(fragNormal, normalize(-lightPos))), 0.0001);"
	"return currentDepth - bias > closestDepth ? 0.0 : 1.0;"
"}"

"vec3 calculate_point_lighting(float moonIntensity)"
"{"
	"vec3 color = vec3(0.0);"

	"for (int i = 0; i < NUM_LIGHTS; i++) {"
		"vec3 pointLightPos = lightPositions[i];"

		"vec3 lightDir = normalize(pointLightPos - fragPos);"

		"float diff = max(dot(fragNormal, lightDir), 0.0);"
		"float attenuation = 1.0 / (1.0 + 0.09 * length(pointLightPos - fragPos));"
		"vec3 lightColor = vec3(1.0, 0.5, 0.0) / NUM_LIGHTS;"

		"color += diff * lightColor * attenuation * (1.0 - moonIntensity);"
	"}"

	"return color;"
"}"

"void main()"
"{"
	"vec3 lightDir = normalize(lightPos);"
	"float moonIntensity = max(dot(lightDir, vec3(0.0, 1.0, 0.0)), 0.0);"
	
	"float diff = max(dot(fragNormal, lightDir), 0.0);"

	"float shadow = shadowCalculation();"
	
	"vec3 ambient = vec3(0.1) * shadow * clamp(1.0 - fragPos.x / 10.0, 0.0, 1.0);"
	"vec3 pointLighting = calculate_point_lighting(moonIntensity * 0.5);"

	"vec3 viewDir = normalize(viewPos - fragPos);"
	"float fresnel = pow(1.0 - max(dot(viewDir, fragNormal), 0.0), 5.0);"
	"vec3 fresnelReflection = vec3(0.3, 0.5, 0.7) * fresnel * moonIntensity * 0.3;"

	"vec3 finalColor = ambient + pointLighting + fresnelReflection;"
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

// --------------------------- ICE SHADERS ---------------------------

static const char iceVertSrc[] = "#version 330 core\n"
"layout(location=0) in vec3 pA;"
"out vec2 texCoords;"
"uniform mat4 model;"
"uniform mat4 view;"
"uniform mat4 reflectionView;"
"uniform mat4 projection;"
"void main()"
"{"
	"vec4 pos=projection*view*model*vec4(pA,1.);"
	"vec4 reflPos=projection*reflectionView*model*vec4(pA,1.);"
	"reflPos/=reflPos.w;"
	"texCoords=(reflPos.xy+1.)/2.;"
	"gl_Position=pos;"
"}";

static const char iceFragSrc[] = "#version 330 core\n"
"out vec4 fragColor;"
"in vec2 texCoords;"
"uniform sampler2D reflection;"
"void main()"
"{"
	"fragColor=texture(reflection,texCoords);"
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

"uniform vec3 lightPos;"

"vec3 calculate_point_lighting(float lightIntensity)"
"{"
	"vec3 color = vec3(0.0);"

	"for (int i = 0; i < NUM_LIGHTS; i++) {"
		"vec3 pointLightPos = lightPositions[i];"

		"vec3 lightDir = normalize(pointLightPos - fragPos);"

		"float diff = max(dot(fragNormal, lightDir), 0.0);"

		"float attenuation = 1.0 / (1.0 + 0.09 * length(pointLightPos - fragPos));"
		"vec3 lightColor = vec3(1.0, 0.5, 0.0) / NUM_LIGHTS;"

		"color += diff * lightColor * attenuation * (1.0 - lightIntensity);"
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

	"float lightIntensity = max(dot(normalize(lightPos), vec3(0.0, 1.0, 0.0)), 0.0);"
	"vec3 ambient = baseAmbient + min(vec3(0.3, 0.3, 0.4) * lightIntensity, 1.0) * 0.05;"

	"vec3 pointLighting = calculate_point_lighting(lightIntensity);"

	"fragColor = vec4(ambient + pointLighting, 1.0);"
"}";

// --------------------------- TREE SHADERS ---------------------------

static const char treeVertSrc[] = "#version 330 core\n"
"layout(location=0) in vec3 aPosition;"
"layout(location=1) in vec3 aNormal;"
"layout (location = 3) in mat4 instanceModel;"

"out vec3 fragPos;"
"out vec3 fragNormal;"

"uniform mat4 model;"
"uniform mat4 view;"
"uniform mat4 projection;"

"void main()"
"{"
	"mat4 modelInstance = model * instanceModel;"

	"vec4 pos = modelInstance * vec4(aPosition, 1.0);"
	"fragPos = pos.xyz;"

	"mat3 normalMatrix = transpose(inverse(mat3(modelInstance)));"
	"fragNormal = normalize(normalMatrix * aNormal);"

	"gl_Position = projection * view * pos;"
"}";

static const char treeFragSrc[] = "#version 430 core\n"
"#define NUM_LIGHTS 11\n"

"layout(std430, binding = 0) buffer StorageBuffer {"
	"vec3 lightPositions[];"
"};"

"out vec4 fragColor;"

"in vec3 fragPos;"
"in vec3 fragNormal;"

"uniform vec3 lightPos;"

"vec3 calculate_point_lighting(vec3 normal, float moonIntensity)"
"{"
	"vec3 color = vec3(0.0);"

	"for (int i = 0; i < NUM_LIGHTS; i++) {"
		"vec3 pointLightPos = lightPositions[i];"

		"vec3 lightDir = normalize(pointLightPos - fragPos);"

		"float diff = max(dot(normal, lightDir), 0.0);"
		"float distance = length(pointLightPos - fragPos);"
		"float fade = 1.0 - smoothstep(0.0, 8.0, distance);"
		"float attenuation = fade / (1.0 + 0.09 * distance);"
		"vec3 lightColor = vec3(1.0, 0.5, 0.0) / NUM_LIGHTS;"

		"color += diff * lightColor * attenuation * (1.0 - moonIntensity);"
	"}"

	"return color;"
"}"

"void main()"
"{"
	"vec3 bumpedNormal = normalize(fragNormal + 0.5 * (vec3( "
		"fract(sin(dot(floor(fragPos.xy * 50.0), vec2(12.9898, 78.233))) * 43758.5453), "
		"fract(sin(dot(floor(fragPos.yz * 50.0), vec2(93.9898, 67.345))) * 43758.5453), "
		"fract(sin(dot(floor(fragPos.zx * 50.0), vec2(56.789, 43.321))) * 43758.5453)"
	") - 0.5));"
	"vec3 lightDir = normalize(lightPos - fragPos);"

	"float moonIntensity = max(dot(bumpedNormal, lightDir), 0.0);"
	"vec3 pointLighting = calculate_point_lighting(bumpedNormal, moonIntensity);"

	"fragColor = vec4(pointLighting * 0.5, 1.0);"
"}";

static const char needleVertSrc[] = "#version 330 core\n"
"layout(location=0) in vec3 needleParameters;"
"uniform mat4 projection;"
"void main()"
"{"
	"float height = needleParameters.x;"
	"float angle = needleParameters.y;"
	"float tilt = needleParameters.z;"
	"vec3 position = vec3(sin(angle) * cos(tilt), sin(tilt), cos(angle) * cos(tilt)) * height;"
	"gl_Position = projection * vec4(position, 1.0);"
"}";

static const char needleFragSrc[] = "#version 330 core\n"
"out vec4 c;"
"void main()"
"{"
	"c=vec4(1.);"
"}";


GLuint debugShader;
GLuint basicShader;

GLuint textShader;
GLuint snoiseShader;
GLuint rnoiseShader;

GLuint atmosphereShader;
GLuint skyShader;

GLuint snowShader;
GLuint shadowSnowShader;
GLuint updateSnowShader;

GLuint iceShader;

GLuint characterShader;
GLuint shadowCharacterShader;

GLuint treeShader;
GLuint shadowTreeShader;
GLuint needleShader;

void initShaders() {
	debugShader = compileShader(debugVertSrc, NULL, NULL, NULL, debugFragSrc);
	basicShader = compileShader(basicVertSrc, NULL, NULL, NULL, basicFragSrc);

	textShader = compileShader(textVertSrc, NULL, NULL, NULL, textFragSrc);
	snoiseShader = compileShader(postVertSrc, NULL, NULL, NULL, snoiseFragSrc);
	rnoiseShader = compileShader(postVertSrc, NULL, NULL, NULL, rnoiseFragSrc);

	atmosphereShader = compileShader(postVertSrc, NULL, NULL, NULL, atmosphereFragSrc);
	skyShader = compileShader(skyVertSrc, NULL, NULL, NULL, skyFragSrc);

	snowShader = compileShader(snowVertSrc, snowTCSSrc, snowTESCSrc, NULL, snowFragSrc);
	shadowSnowShader = compileShader(snowVertSrc, NULL, NULL, NULL, basicFragSrc);
	updateSnowShader = compileShader(postVertSrc, NULL, NULL, NULL, updateSnowFragSrc);

	iceShader = compileShader(iceVertSrc, NULL, NULL, NULL, iceFragSrc);

	characterShader = compileShader(characterVertSrc, NULL, NULL, NULL, characterFragSrc);
	shadowCharacterShader = compileShader(characterVertSrc, NULL, NULL, NULL, basicFragSrc);

	treeShader = compileShader(treeVertSrc, NULL, NULL, NULL, treeFragSrc);
	shadowTreeShader = compileShader(treeVertSrc, NULL, NULL, NULL, basicFragSrc);
	needleShader = compileShader(needleVertSrc, NULL, NULL, NULL, needleFragSrc);
}

void cleanupShaders() {
	glDeleteProgram(debugShader);

	glDeleteProgram(textShader);
	glDeleteProgram(snoiseShader);
	glDeleteProgram(rnoiseShader);

	glDeleteProgram(atmosphereShader);
	glDeleteProgram(skyShader);

	glDeleteProgram(snowShader);
	glDeleteProgram(shadowSnowShader);
	glDeleteProgram(updateSnowShader);

	glDeleteProgram(iceShader);

	glDeleteProgram(characterShader);
	glDeleteProgram(shadowCharacterShader);

	glDeleteProgram(treeShader);
	glDeleteProgram(needleShader);
}