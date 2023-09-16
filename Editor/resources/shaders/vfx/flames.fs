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


//Texture
uniform sampler2D u_Texture;
uniform vec2 u_Texture_Velocity;

//Transp
uniform sampler2D u_TranspTexture;
uniform sampler2D u_TranspMask;
uniform vec2 u_TranspTex_Velocity;
uniform vec2 u_TranspTexMask_Velocity;

//Fresnel
uniform vec2 u_FresnelRange;
uniform vec4 u_FresnelColor;

//Internal
uniform vec3 u_CameraPosition;

void main()
{

    vec2 texVel = vec2(u_Time * u_Texture_Velocity.x, u_Time * u_Texture_Velocity.y);
    vec2 texTransVel = vec2(u_Time * u_TranspTex_Velocity.x, u_Time * u_TranspTex_Velocity.y);
    vec2 texTransMaskVel = vec2(u_Time * u_TranspTexMask_Velocity.x, u_Time * u_TranspTexMask_Velocity.y);

    vec3 dir = u_CameraPosition - LocalPos;

    float dotEyeNormal = dot(normalize(dir),normalize(Normal));
    dotEyeNormal = abs(dotEyeNormal);

    vec4 colorTex  =  texture(u_Texture,TexCoord + texVel) * u_Color;   
    vec4 colorTransp = texture2D(u_TranspTexture, TexCoord + texTransVel);
    vec4 colorTranspMask = texture2D(u_TranspMask, TexCoord + texTransMaskVel);

    // float grayscale = dot(colorTransp.rgb, vec3(0.299, 0.587, 0.114));

    vec4 finalColor = mix(u_FresnelColor,colorTex,smoothstep(u_FresnelRange.x,u_FresnelRange.y,dotEyeNormal));    

    FragColor = finalColor;
    FragColor.a = finalColor.a * colorTransp.r * colorTranspMask.r; //grayscale;

    // check whether result is higher than some threshold, if so, output as bloom threshold color
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
	else
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}