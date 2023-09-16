#version 330
 #extension GL_ARB_explicit_uniform_location : enable

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
uniform vec4 u_ColorTex2;
//=====================
uniform vec3 u_CameraPosition;
uniform sampler2D u_Texture;
uniform sampler2D u_Texture2;
uniform sampler2D u_DiscardTex;
uniform vec2 u_FresnelRange;
uniform vec4 u_FresnelColor;

void main()
{
    //calculate dissolve

    // if(u_LifeTime > 0.9)
    // {
    //     float dissolveAmount = (u_LifeTime - 0.9) * 10.0;

    //     float dissolve = texture2D(u_DiscardTex, TexCoord).r;
    //     dissolve = dissolve *  0.999;

    //     float visible = dissolve - dissolveAmount;

    //     if(visible < 0)
    //         discard;
    // }
    // // Calculate the center of the texture
    // vec2 center = vec2(0.25, 0.25);

    // // Calculate the rotation matrix
    // float s = 0;
    // float c = cos(u_LifeTime);
    // mat2 rotationMatrix = mat2(c, -s, s, c);

    // // Apply the rotation to the texture coordinate
    // vec2 rotatedCoord = rotationMatrix * (TexCoord - center) + center;


    // //color tex 1
    // vec4 color1 = texture2D(u_Texture, TexCoord) * u_Color;
    // //get the second texture white part
    // vec4 color2 = texture2D(u_Texture2, rotatedCoord) * u_ColorTex2;

    // // Check if texture color is black
    // bool isBlack = color1.r == 0.0 && color1.g == 0.0 && color1.b == 0.0;

    // if(isBlack) 
    //     color1 = vec4(1,1,1,0);


    // //ignore the black pixels
    // isBlack = color2.r <= 0.5 && color2.g <= 0.5  && color2.b <= 0.5 ;

    // if(isBlack)
    //     color2 = vec4(1,1,1,0);  

    // //alpha blend
    // vec4 combinedColor = mix(color1, color2, color2.a);

    // //calculate fresnel
    // vec3 dir = u_CameraPosition - LocalPos;
    // float dotEyeNormal = dot(normalize(dir),normalize(Normal));
    // dotEyeNormal = abs(dotEyeNormal);

    // vec4 finalColor = mix( u_FresnelColor, combinedColor  ,smoothstep(u_FresnelRange.x,u_FresnelRange.y,dotEyeNormal));

    // FragColor = finalColor;

    // // check whether result is higher than some threshold, if so, output as bloom threshold color
    // float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    // if(brightness > 1.0)
    //     BrightColor = vec4(FragColor.rgb, 1.0);
	// else
	// 	BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}