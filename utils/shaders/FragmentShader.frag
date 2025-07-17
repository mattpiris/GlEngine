#version 330 core
uniform vec3 ambientColor;
out vec4 FragColor;

void main() {
	float strenght = 1.0f;
	FragColor = vec4(ambientColor * strenght, 1.0f);
}