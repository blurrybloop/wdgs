#version 330 core

uniform samplerCube surf_emission;

layout(std140) uniform LightSource
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
} ls;

layout(std140) uniform Camera
{
	vec3 position;

} cam;

in vec3 normal;
in vec3 fragPos;

out vec4 color;

void main(void)
{

	//Emission
	vec3 emission = vec3(texture(surf_emission, normal)) * ls.diffuse;

	float distance = length(fragPos - cam.position);
	emission *= 1.0f / (1.0 + 1E-10 * distance + 1E-22 * (distance * distance));


	color = vec4(emission, 1.0);
}
