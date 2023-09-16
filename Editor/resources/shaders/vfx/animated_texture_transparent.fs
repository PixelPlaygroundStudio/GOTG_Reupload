#version 330
 #extension GL_ARB_explicit_uniform_location : enable

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 LocalPos;

//====================================
// all vfx must have this unifroms
//lifetime form 0 to 1,  100% normalized
uniform float u_LifeTime;
uniform float u_Time;
uniform vec4 u_Color;
//=====================

uniform vec2 u_Velocity;
uniform vec2 u_Offset;
uniform sampler2D u_Texture;
uniform sampler2D u_TransparencyTexture;
uniform sampler2D u_FadeTexture;

void main()
{
    vec2 offset = vec2(u_Offset.x + u_Time * u_Velocity.x, u_Offset.y + u_Time * u_Velocity.y);
    

    vec2 TexCoords = TexCoord;
     TexCoords += offset;

    vec4 colorTex = texture2D(u_Texture, TexCoords) * u_Color;
    vec4 colorTransp = texture2D(u_TransparencyTexture, TexCoords);
    colorTransp.a = u_Color.a;

    float grayscale = dot(colorTransp.rgb, vec3(0.299, 0.587, 0.114));    

    FragColor = colorTex;
    FragColor.a = grayscale;


    // check whether result is higher than some threshold, if so, output as bloom threshold color
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
	else
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}