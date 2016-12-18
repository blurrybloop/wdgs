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
	vec3 tc;
	vec3 normal;
	vec3 fragPos;
} fs_in;

out vec4 color;

void main(void)
{
	// Ambient
	vec4 ambient = vec4(ls.ambient, 1.0) * texture(surf_diffuse, fs_in.tc);

	// Diffuse 

	vec3 norm = normalize(fs_in.normal);
	vec3 lightDir = normalize(ls.position - fs_in.fragPos);
	float diff = max(dot(lightDir, norm), 0.0);
	vec4 surface = texture(surf_diffuse, fs_in.tc);
	vec4 clouds = vec4(1.0, 1.0, 1.0, texture(surf_clouds, fs_in.tc));
	vec4 diffuse = vec4(ls.diffuse * diff, 1.0) * (surface * (1 - clouds.a) + clouds * clouds.a);

	float distance = length(fs_in.fragPos - ls.position);
	diffuse *= 1.0f / (1.0 + 1E-21 * distance + 4E-22 * (distance * distance));

	// Specular
	vec3 viewDir = normalize(cam.position - fs_in.fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
	vec4 specular = vec4(ls.specular * spec, 1.0) * texture(surf_specular, fs_in.tc);

	//Emission
	vec4 emission = pow(1.0 - diff, 16) * texture(surf_emission, fs_in.tc);

	color = ambient + diffuse + emission;
}
