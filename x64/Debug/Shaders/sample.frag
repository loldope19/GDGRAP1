#version 330 core
// Texture to be passed
uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D norm_tex;

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

in mat3 TBN;

uniform vec3 objColor; // Object Color
out vec4 FragColor; // Color pixel

void main(){

	vec4 pixelColor = texture(tex0, texCoord);
	// Alpha Cutoff Shader
	if (pixelColor.a < 0.1) {
		discard;
	}

	// FragColor = vec4(objColor, 1.0f);
	vec4 texColor0 = texture(tex0, texCoord);			// Brickwall Texture
	vec4 texColor1 = texture(tex1, texCoord);			// Yae Texture
	vec4 texColor = mix(texColor0, texColor1, 0.5);		// Both textures blended at semi-transparency (0.5 as per direction)

	// Assigns the texture color using the function
	FragColor = texColor;
	
	vec3 normal = texture(norm_tex, texCoord).rgb;
	normal = normalize(normal * 2.0 - 1.0);
	normal = normalize(TBN * normal);

	vec3 lightDir = normalize(-lightPos);		// Utilized Direction Light instead of Point Light for a brighter model

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	vec3 ambientCol = ambientColor * ambientStr;

	vec3 viewDir = normalize(cameraPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	
	float spec = pow(max(dot(reflectDir, viewDir), 0.1), specPhong);
	vec3 specColor = spec * specStr * lightColor;

	FragColor = vec4(specColor + ambientCol + diffuse, 1.0) * FragColor;
}