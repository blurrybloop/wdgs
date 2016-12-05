#version 330 core

out vec4 color;
in vec2 texCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(scene, texCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, texCoords).rgb;
    hdrColor += bloomColor; // additive blending
    // tone mapping
    vec3 result = hdrColor / (hdrColor + vec3(1.0));
    // also gamma correct while we're at it       
    //result = pow(result, vec3(1.0 / gamma));
	color = vec4(result, 1.0f);
}