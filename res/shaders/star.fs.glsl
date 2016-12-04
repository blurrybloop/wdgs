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

out vec4 color;

void main(void)
{
	//Emission
	vec4 emission = texture(surf_emission, normal);
	color = emission;
}
