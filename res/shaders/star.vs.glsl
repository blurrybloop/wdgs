#version 330 core

uniform mat4 model;
uniform mat4 mvp;

layout (location = 0) in vec3 position;

out vec3 normal;

void main(void)
{
	normal = mat3(model) * position;

    gl_Position = mvp * vec4(position, 1.0);
}
