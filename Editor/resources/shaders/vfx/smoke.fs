#version 330


layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

const int MAX_POINT_LIGHTS = 32;
const int MAX_SPOT_LIGHTS = 32;

struct BaseLight
{
    vec3 Color;
    float AmbientIntensity;
    float DiffuseIntensity;
};

struct DirectionalLight
{
    BaseLight Base;
    vec3 Direction;
};

in vec2 TexCoord;
in vec3 Normal;
in vec3 LocalPos;
in vec4 LightSpacePos;

//====================================
// all vfx must have this unifroms
//lifetime form 1 to 0,  100% normalized
uniform float u_LifeTime;
uniform vec4 u_Color;
//=====================
uniform float u_DissolveAmount;
uniform float u_OutDissolve;
uniform float u_EnterDissolve;

uniform sampler2D u_Texture;
uniform sampler2D u_DiscardTex;

uniform DirectionalLight u_DirectionalLight;
uniform vec3 u_CameraPosition;

void main()
{
  
    //calculate dissolve
    if(u_LifeTime > u_OutDissolve)
    {
        float dissolveAmount = (u_LifeTime - u_OutDissolve) * 10.0;

        vec4 displacement = texture2D(u_DiscardTex, TexCoord);

        // Scale the displacement vector by the factor.
        displacement.xy *= u_LifeTime;
        // Add the scaled displacement vector to the texture coordinates.
        vec2 displacedCoords = TexCoord + displacement.xy;

        float dissolve = texture2D(u_DiscardTex, displacedCoords).r;
        dissolve = dissolve *  0.999;

        float visible = dissolve - dissolveAmount;

        if(visible < 0)
            discard;
    }
    if(u_LifeTime < u_EnterDissolve)
    {
        float dissolveAmount = u_LifeTime * 10.0;

        float dissolve = texture2D(u_DiscardTex, TexCoord).r;
        dissolve = dissolve *  0.999;

        float visible = dissolve - dissolveAmount;

        if(visible < 0)
            discard;
    }

        

    float dotCamNormal = dot(normalize(u_DirectionalLight.Direction),normalize(Normal));

    vec4 finalColor = texture2D(u_Texture, TexCoord) * u_Color;

    if(dotCamNormal < 0)
    {
        FragColor = finalColor * vec4(0.5,0.5,0.5,1);
    }
    else
    {
         FragColor = finalColor;
    }

    // check whether result is higher than some threshold, if so, output as bloom threshold color
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
	else
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}