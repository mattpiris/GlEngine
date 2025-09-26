#version 330 core

in vec4 aColor;
in vec3 normal;

uniform vec3 lightColor;

out vec4 FragColor;

void main() {
	vec3 ambient = lightColor * vec3(aColor);
	
	vec3 lightDir = normalize(vec3(1.0, -1.0, 1.0)); // Directional light
    float diff = max(dot(normalize(normal), lightDir), 0.0);
    vec3 diffuse = diff * lightColor * vec3(aColor);

	vec3 result = ambient + diffuse;
    FragColor = vec4(result, 1.0);
}