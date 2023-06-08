#version 330 core
// Texture to be passed
uniform sampler2D tex0;

// Should receive texCoord from the vertex shader
in vec2 texCoord;

// uniform vec3 objColor; // Object Color
out vec4 FragColor; // Color pixel

void main(){

	//FragColor = vec4(objColor, 1.0f);

	// Assigns the texture color using the function
	FragColor = texture(tex0, texCoord);
}