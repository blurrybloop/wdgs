#version 330 core

uniform mat4 model;
uniform mat4 mvp;

layout (location = 0) in vec3 position;

out VS_OUT
{
	vec3 tc;
	vec3 normal;
	vec3 fragPos;
} vs_out;

void main(void)
{
	vs_out.tc = position;
	vs_out.normal = mat3(model) * position;
	vs_out.fragPos = vec3(model * vec4(position, 1.0f));

    gl_Position = mvp * vec4(position, 1.0);
}
