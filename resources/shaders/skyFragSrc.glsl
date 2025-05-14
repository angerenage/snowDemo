#version 330 core
in vec3 fragPos;out vec4 fragColor;uniform samplerCube skybox;void main(){fragColor=texture(skybox,fragPos);gl_FragDepth=1.;}