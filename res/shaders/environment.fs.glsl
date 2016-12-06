#version 330 core

uniform samplerCube environment;

in vec3 tc;

out vec4 color;

void main(void)
{
	color =  texture(environment, tc);
}
