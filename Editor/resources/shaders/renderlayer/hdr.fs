#version 330 core

out vec4 FragColor;

in vec2 s_TexCoord;

// Texture
uniform sampler2D u_LayerTex;

uniform sampler2D u_HdrBufferTexture;
uniform bool u_Hdr;
uniform float u_Exposure;

void main()
{
	const float gamma = 2.2;
    vec3 hdrColor = texture(u_HdrBufferTexture, s_TexCoord).rgb;
    if(u_Hdr)
    {
        vec3 result = vec3(1.0) - exp(-hdrColor * u_Exposure);
        // also gamma correct while we're at it       
        result = pow(result, vec3(1.0 / gamma));
        FragColor = vec4(result, 1.0);
    }
    else
    {
        vec3 result = pow(hdrColor, vec3(1.0 / gamma));
        FragColor = vec4(result, 1.0);
    }
}