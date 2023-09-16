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

uniform sampler2D u_Texture;
uniform sampler2D u_DiscardTexture;
uniform sampler2D u_TransparencyTexture;

uniform vec2 u_FresnelRange;
uniform vec4 u_FresnelColor;
uniform float u_StartDissolve;
uniform vec2 u_OffsetMultiplier;

uniform vec3 u_CameraPosition;


void main()
{
    if(u_LifeTime > u_StartDissolve)
    {
        float dissolveAmount = (u_LifeTime - u_StartDissolve) * 10.0;

        vec4 texDissolve = texture2D(u_DiscardTexture, TexCoord);
        float dissolveGrayscale = dot(texDissolve.rgb, vec3(0.299, 0.587, 0.114));

        float visible = dissolveGrayscale - dissolveAmount;

        if(visible < 0)
            discard;
    }


    vec2 offset = vec2(u_Time * u_OffsetMultiplier.x, u_Time * u_OffsetMultiplier.y);

    vec3 dir = u_CameraPosition - LocalPos;

    float dotEyeNormal = dot(normalize(dir),normalize(Normal));
    dotEyeNormal = abs(dotEyeNormal);

    vec4 colorTex  =  texture(u_Texture,TexCoord + offset) * u_Color;   
    vec4 colorTransp = texture2D(u_TransparencyTexture, TexCoord);

    float grayscale = dot(colorTransp.rgb, vec3(0.299, 0.587, 0.114));

    vec4 finalColor = mix(u_FresnelColor,colorTex,smoothstep(u_FresnelRange.x,u_FresnelRange.y,dotEyeNormal));

    vec2 hologramTexCoords = TexCoord;

    // float displacement = u_Amplitude * sin(TexCoord.y * u_Frequency * u_LifeTime);
    hologramTexCoords += offset;



    FragColor = finalColor; 
    FragColor.a = grayscale;

    // check whether result is higher than some threshold, if so, output as bloom threshold color
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
	else
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}