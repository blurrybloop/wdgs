#version 330 core

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

uniform vec4 athmoColor;

in VS_OUT
{
	vec3 normal;
	vec3 fragPos;
} fs_in;

out vec4 color;

void main(void)
{
	// Ambient
	vec4 ambient = vec4(ls.ambient, 1.0) * athmoColor;

	// Diffuse 
	vec3 norm = normalize(fs_in.normal);
	vec3 frag = fs_in.fragPos;

	vec3 lightDir = normalize(ls.position - fs_in.fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec4 diffuse = vec4(ls.diffuse * diff, 1.0) * athmoColor;

	//vec3 huy = (fs_in.projPos);
	//huy.z = centerPos.z;
	//huy = normalize(huy);
	//huy.z =-1.0;

	//float theta = abs(fs_in.projPos.z - centerPos.z);
	//float epsilon = outerAngle - innerAngle;
	//float intensity = clamp((outerAngle - theta) / epsilon, 0.0, 1.0);

	color = ambient + diffuse;
	//color.a *= intensity;
}
