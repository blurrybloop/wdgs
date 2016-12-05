#version 330 core

out vec4 color;

in vec2 texCoords;

uniform sampler2D hdrBuffer;

void main(void)
{
	vec3 hdrColor = texture(hdrBuffer, texCoords).rgb;
	vec3 result = hdrColor / (hdrColor + vec3(1.0));
	color = vec4(hdrColor, 1.0);
}
