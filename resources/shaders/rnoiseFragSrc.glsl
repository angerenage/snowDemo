#version 330 core
in vec2 fragPos;out vec4 fragColor;void main(){fragColor=vec4(fract(sin(dot(fragPos,vec2(12.9898,78.233)))*43758.5453123));}