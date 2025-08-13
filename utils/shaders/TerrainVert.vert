#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform vec4 color;

uniform mat4 view;
uniform mat4 projection;

out vec4 aColor;
out vec3 normal;

void main() {
	gl_Position = projection * view * vec4(aPos, 1.0f);
	aColor = color;
	normal = aNormal;
}