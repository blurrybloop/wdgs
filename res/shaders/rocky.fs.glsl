#version 330 core

uniform samplerCube surf_diffuse;
uniform samplerCube surf_specular;
uniform samplerCube surf_emission;

uniform vec3 cameraPos;

in VS_OUT
{
	vec3 normal;
	vec3 fragPos;
} fs_in;

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Light light;

out vec4 color;

void main(void)
{
	// Ambient
	vec4 ambient = vec4(light.ambient, 1.0) * texture(surf_diffuse, fs_in.normal);

	// Diffuse 

	vec3 norm = normalize(fs_in.normal);
	vec3 lightDir = normalize(light.position - fs_in.fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec4 diffuse = vec4(light.diffuse * diff, 1.0) * texture(surf_diffuse, fs_in.normal);

	// Specular
	vec3 viewDir = normalize(cameraPos - fs_in.fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec4 specular = vec4(light.specular * spec, 1.0) * texture(surf_specular, fs_in.normal);

	//Emission
	vec4 emission = pow(1.0 - diff, 16) * texture(surf_emission, fs_in.normal);

	color = ambient + diffuse + specular + emission;
}
