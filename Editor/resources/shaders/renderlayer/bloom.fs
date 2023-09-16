#version 330 core
out vec4 FragColor;

in vec2 s_TexCoord;

uniform sampler2D u_Scene;
uniform sampler2D u_BloomBlur;
uniform bool u_Bloom;
uniform float u_Exposure;

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(u_Scene, s_TexCoord).rgb;      
    vec3 bloomColor = texture(u_BloomBlur, s_TexCoord).rgb;
    if(u_Bloom)
        hdrColor += bloomColor + vec3(1,0,0); // additive blending
    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * u_Exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);

}