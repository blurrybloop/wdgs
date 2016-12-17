#version 330 core

uniform samplerCube environment;
uniform bool useTex;

in vec3 tc;

out vec4 color;

void main(void)
{
	if (useTex)
		color = texture(environment, tc);
	else
		vec4(0.0, 0.0, 0.0, 1.0);
}
