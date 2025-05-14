#version 330 core
in vec2 fragPos;uniform sampler2D previousDepthMap;uniform vec2 offset;void main(){gl_FragDepth=texture(previousDepthMap,(fragPos+1.)*.5+offset).x;}