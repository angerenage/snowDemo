#version 330 core
in vec3 color;out vec4 fragColor;void main(){vec2 c=gl_PointCoord-.5;if(length(c)>.5)discard;fragColor=vec4(pow(color,vec3(1.5)),1);}