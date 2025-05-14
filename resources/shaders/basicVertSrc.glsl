#version 330 core
layout(location=0)in vec3 position;out vec2 fragPos;uniform mat4 model,view,projection;void main(){fragPos=(position.xy+1.)/2.;gl_Position=projection*view*model*vec4(position,1);}