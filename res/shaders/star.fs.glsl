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

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 brightColor;

void main(void)
{
	//Emission
	vec4 emission = texture(surf_emission, normal) * 10.0;
	fragColor = emission;

	float brightness = dot(vec3(fragColor), vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0)
		brightColor = fragColor;
	else
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
