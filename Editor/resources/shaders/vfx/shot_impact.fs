#version 330

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 LocalPos;

//====================================
// all vfx must have this unifroms
//lifetime form 1 to 0,  100% normalized
uniform float u_LifeTime;
uniform vec4 u_Color;
//=====================
uniform vec3 u_CameraPosition;
uniform sampler2D u_Texture;

void main()
{

    float alpha = texture2D(u_Texture, TexCoord).a;

    if(alpha < 0.01)
        discard;

    float isWhite = texture2D(u_Texture, TexCoord).r;


    vec4 color = u_Color;

    if(isWhite > 0.9)
    {
        color = vec4 (1,1,1,u_Color.a);
    }

    FragColor = color;

    // check whether result is higher than some threshold, if so, output as bloom threshold color
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
	else
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}