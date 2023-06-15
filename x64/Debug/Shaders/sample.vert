#version 330 core

layout(location = 0) in vec3 aPos;
// The tex coord / UV is at 0
// Accesses the UV and assigns it to aTex
layout(location = 2) in vec2 aTex;

// Passes the tex coord to the fragment shader
out vec2 texCoord;

uniform mat4 transform;
uniform mat4 projection;
uniform mat4 view;

void main(){
	gl_Position = projection * view * transform * vec4(aPos, 1.0);
	texCoord = aTex;
}