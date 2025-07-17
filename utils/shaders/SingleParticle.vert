#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec3 color;

uniform vec3 offset;
uniform float size;
uniform mat4 projection;
uniform mat4 view;

out vec3 outColor;

void main()
{
    vec3 worldPosition = vec3(position, 0.0) * size + offset;
    gl_Position = projection * view * vec4(worldPosition, 1.0);
    outColor = color;
}