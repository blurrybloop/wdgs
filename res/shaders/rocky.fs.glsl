#version 330 core

uniform samplerCube surf_diffuse;
uniform samplerCube surf_specular;
uniform samplerCube surf_emission;
uniform samplerCube surf_clouds;

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

in VS_OUT
{
	vec3 normal;
	vec3 fragPos;
} fs_in;


layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 brightColor;

void main(void)
{
	// Ambient
	vec4 ambient = vec4(ls.ambient, 1.0) * texture(surf_diffuse, fs_in.normal);

	// Diffuse 

	vec3 norm = normalize(fs_in.normal);
	vec3 lightDir = normalize(ls.position - fs_in.fragPos);
	float diff = max(dot(lightDir, norm), 0.0);
	vec4 surface = texture(surf_diffuse, fs_in.normal);
	vec4 clouds = vec4(1.0, 1.0, 1.0, texture(surf_clouds, fs_in.normal));
	vec4 diffuse = vec4(ls.diffuse * diff, 1.0) * (surface * (1 - clouds.a) + clouds * clouds.a);

	// Specular
	vec3 viewDir = normalize(cam.position - fs_in.fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
	vec4 specular = vec4(ls.specular * spec, 1.0) * texture(surf_specular, fs_in.normal);

	//Emission
	vec4 emission = pow(1.0 - diff, 16) * texture(surf_emission, fs_in.normal);

	fragColor = ambient + diffuse + emission;
	brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
