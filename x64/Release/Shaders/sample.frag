#version 330 core

uniform vec3 objColor; // Object Color
out vec4 FragColor; // Color pixel

void main(){

	FragColor = vec4(objColor, 1.0f);
}