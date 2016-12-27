#version 330 core

uniform mat4 model;
uniform mat4 mvp;

layout (location = 0) in vec3 position;

out vec3 normal;
out vec3 fragPos;

void main(void)
{
	normal = mat3(model) * position;
	fragPos = vec3(model * vec4(position, 1.0f));

    gl_Position = mvp * vec4(position, 1.0);
}
