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

uniform vec3 objColor; // Object Color
out vec4 FragColor; // Color pixel

void main(){

	// FragColor = vec4(objColor, 1.0f);

	// Assigns the texture color using the function
	FragColor = texture(tex0, texCoord);

	vec3 normal = normalize(normCoord);
	vec3 lightDir = normalize(-lightPos);		// Main Implementation of Direction Light (basically normalizing the (negative) light Position)

	// Gets the distance between the light and the position of the fragment
	// float fDistance = length(lightPos - fragPos);
	// Intensity of the light with regards to the distance of the fragment to the light source
	// Equation is 1 / Distance^2
	// float fIntensity = 1 / (fDistance * fDistance);

	float diff = max(dot(normal, lightDir), 0.0);
	// vec3 diffuse = diff * lightColor * fIntensity;		// Light intensity factored on the diffuse component of the object
	vec3 diffuse = diff * lightColor;

	vec3 ambientCol = ambientColor * ambientStr;

	vec3 viewDir = normalize(cameraPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);

	float spec = pow(max(dot(reflectDir, viewDir), 0.1), specPhong);
	//vec3 specColor = spec * specStr * lightColor * fIntensity;		// Light intensity factored on the specular component of the object, 
	vec3 specColor = spec * specStr * lightColor;

	FragColor = vec4(specColor + ambientCol + diffuse, 1.0) * texture(tex0, texCoord);
}