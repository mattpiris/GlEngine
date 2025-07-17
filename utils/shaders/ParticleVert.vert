#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec3 offset;
layout (location = 3) in float size;

uniform mat4 projection;
uniform mat4 view;

out vec4 outColor;
out float life;

void main()
{
	vec3 camRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 camUp    = vec3(view[0][1], view[1][1], view[2][1]);

    // Build the billboarded position
    vec3 worldPosition = offset
                       + camRight * position.x * size
                       + camUp    * position.y * size;
	gl_Position = projection * view * vec4(worldPosition, 1.0f);
	outColor = color;
}