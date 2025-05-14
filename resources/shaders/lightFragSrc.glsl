#version 430 core
out vec4 fragColor;void main(){vec2 m=gl_PointCoord*2.-1.;float v=length(m);if(v>1.)discard;fragColor=vec4(mix(vec3(0,.5,1),vec3(1),1.-v),pow(1.-v,3.));}