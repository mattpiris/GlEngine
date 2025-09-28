#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;

out vec4 aColor;
void main() {
    gl_Position = projection * view * vec4(aPos, 1.0);
    aColor = vec4(1.0f, 0.0f, 0.5f, 1.0f);
}