#version 330 core

uniform samplerCube surf_emission;

uniform vec3 cameraPos;

in vec3 normal;

out vec4 color;

void main(void)
{
	//Emission
	vec4 emission = texture(surf_emission, normal);
	color = emission;
}
