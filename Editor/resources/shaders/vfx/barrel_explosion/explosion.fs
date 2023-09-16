#version 330


layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 LocalPos;
in mat4 View;

//====================================
// all vfx must have this unifroms
//lifetime form 1 to 0,  100% normalized
uniform float u_LifeTime;
uniform vec4 u_Color;
//=====================
uniform vec3 u_CameraPosition;
uniform sampler2D u_Texture;
uniform sampler2D u_DiscardTex;
uniform vec2 u_FresnelRange;
uniform vec4 u_FresnelColor;
uniform float u_StartDissolve;

void main()
{
    //calculate dissolve

    if(u_LifeTime > u_StartDissolve)
    {
        float dissolveAmount = (u_LifeTime - u_StartDissolve) * 10.0;

        float dissolve = texture2D(u_DiscardTex, TexCoord).r;
        dissolve = dissolve *  0.999;

        float visible = dissolve - dissolveAmount;

        if(visible < 0)
            discard;
    }

    vec3 dir = u_CameraPosition - LocalPos;

    float dotEyeNormal = dot(normalize(dir),normalize(Normal));
    dotEyeNormal =abs(dotEyeNormal);

    vec4 colorTex  =  texture(u_Texture,TexCoord) * u_Color;   

    vec4 finalColor = mix(u_FresnelColor,colorTex,smoothstep(u_FresnelRange.x,u_FresnelRange.y,dotEyeNormal));

    FragColor = finalColor;

    // check whether result is higher than some threshold, if so, output as bloom threshold color
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
	else
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}