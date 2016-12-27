#version 330 core

out vec4 color;

in vec2 texCoords;

uniform sampler2D hdrBuffer;

void main(void)
{
	const float exposure = 0.7;

	vec4 c = texelFetch(hdrBuffer, ivec2(gl_FragCoord.xy), 0);
	c.rgb = vec3(1.0) - exp(-c.rgb * exposure);
	color = c;
}
