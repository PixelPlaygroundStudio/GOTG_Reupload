#version 330


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
uniform vec2 u_OffsetMultiplier;
uniform vec2 u_Offset;
uniform vec3 u_CameraPosition;
uniform sampler2D u_Texture;
uniform sampler2D u_TransparencyTexture;

void main()
{

     vec2 timeOffset = vec2(u_Time * u_OffsetMultiplier.x, u_Time * u_OffsetMultiplier.y);
    

    vec2 offsetTexCoord = u_Offset + TexCoord + timeOffset;
    
    vec4 colorTex = texture2D(u_Texture, offsetTexCoord) * u_Color;
    vec4 colorTransp = texture2D(u_TransparencyTexture, u_Offset + TexCoord);
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