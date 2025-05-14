#version 330 core
in float id;out vec4 fragColor;uniform float time;float f(){float f=id;return fract(sin(f*.1)*1e4)*.5+f/1e2;}void main(){if(f()>time*7.)discard;fragColor=vec4(1);}