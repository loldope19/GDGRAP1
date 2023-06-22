#version 330 core
// Texture to be passed
uniform sampler2D tex0;

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform vec3 cameraPos;
uniform float specStr;
uniform float specPhong;

uniform float ambientStr;
uniform vec3 ambientColor;

// Should receive texCoord from the vertex shader
in vec2 texCoord;
in vec3 normCoord;
in vec3 fragPos;

// uniform vec3 objColor; // Object Color
out vec4 FragColor; // Color pixel

void main(){

	//FragColor = vec4(objColor, 1.0f);

	// Assigns the texture color using the function
	// FragColor = texture(tex0, texCoord);

	vec3 normal = normalize(normCoord);
	vec3 lightDir = normalize(lightPos - fragPos);

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	vec3 ambientCol = ambientColor * ambientStr;

	vec3 viewDir = normalize(cameraPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);

	float spec = pow(max(dot(reflectDir, viewDir), 0.1), specPhong);
	vec3 specColor = spec* specStr * lightColor;

	FragColor = vec4(specColor + diffuse + ambientCol, 1.0) * texture(tex0, texCoord);
}