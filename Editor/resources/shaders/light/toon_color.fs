#version 330
const int MAX_POINT_LIGHTS = 32;
const int MAX_SPOT_LIGHTS = 32;
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 LocalPos;
in vec4 LightSpacePos;
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

struct Attenuation
{
    float Constant;
    float Linear;
    float Exp;
};

struct PointLight
{
    BaseLight Base;
    vec3 LocalPos;
    Attenuation Atten;
};

struct SpotLight
{
    PointLight Base;
    vec3 Direction;
    float Cutoff;
};




uniform DirectionalLight u_DirectionalLight;
uniform int u_NumPointLights;
uniform PointLight[MAX_POINT_LIGHTS] u_PointLights;
uniform int u_NumSpotLights;
uniform SpotLight[MAX_SPOT_LIGHTS] u_SpotLights;
uniform vec3 u_CameraPosition;

uniform vec4 u_Color;
uniform sampler2D u_ShadowMap;
uniform int u_ToonLevels = 4;
uniform float u_RimLightPower = 4.0;
uniform float u_SpecularValue;
uniform vec4 u_MatAmbientColor;
uniform vec4 u_MatDiffuseColor;
uniform vec4 u_MatSpecularColor;

float ToonScaleFactor = 1.0f / u_ToonLevels;

float CalcShadowFactor()
{
    vec3 projCoords = LightSpacePos.xyz / LightSpacePos.w;
    vec2 UVCoords;
    UVCoords.x = 0.5 * projCoords.x + 0.5;
    UVCoords.y = 0.5 * projCoords.y + 0.5;
    float z = 0.5 * projCoords.z + 0.5;
    float depth = texture(u_ShadowMap, UVCoords).x;

    float bias = 0.0025;

    if(depth + bias < z)
        return 0.5;
    else
        return 1.0;
}

float CalcRimLightFactor(vec3 pixelToCamera, vec3 normal)
{
    float rimFactor = dot(pixelToCamera, normal);
    rimFactor = 1.0 - rimFactor;
    rimFactor = max(0.0, rimFactor);
    rimFactor = pow(rimFactor, u_RimLightPower);
    return rimFactor;
}

vec4 CalcLightInternal(BaseLight light, vec3 direction, vec3 normal)
{
  vec4 ambientColor = vec4(light.Color, 1.0f) * light.AmbientIntensity * u_MatAmbientColor;

    float diffuseFactor = dot(normal, -direction);

    vec4 diffuseColor = vec4(0, 0, 0, 0);
    vec4 specularColor = vec4(0, 0, 0, 0);
    vec4 rimColor = vec4(0, 0, 0, 0);

    if(diffuseFactor > 0) 
    {
        diffuseFactor = floor(diffuseFactor * u_ToonLevels) * ToonScaleFactor;

        diffuseColor = vec4(light.Color, 1.0f) * light.DiffuseIntensity * u_MatDiffuseColor * diffuseFactor;

        vec3 pixelToCamera = normalize(u_CameraPosition - LocalPos);

        float rimFactor = CalcRimLightFactor(pixelToCamera, normal);
        rimColor = diffuseColor * rimFactor;
    }

   return (ambientColor * (diffuseColor + specularColor + rimColor));
}

vec4 CalcDirectionalLight(vec3 normal)
{
    return CalcLightInternal(u_DirectionalLight.Base, u_DirectionalLight.Direction, normal);
}

vec4 CalcPointLight(PointLight light, vec3 normal)
{
    vec3 lightDirection = LocalPos - light.LocalPos;
    float distance = length(lightDirection);
    lightDirection = normalize(lightDirection);

    vec4 color = CalcLightInternal(light.Base, lightDirection, normal);

    float attenuation = light.Atten.Constant +
                        light.Atten.Linear * distance *
                        light.Atten.Exp * distance * distance;

    return color / attenuation;
}
vec4 CalcSpotLight(SpotLight light, vec3 normal)
{
    vec3 lightToPixel = normalize(LocalPos - light.Base.LocalPos);
    float spotFactor = dot(lightToPixel, light.Direction);

    if(spotFactor > light.Cutoff)
    {
        vec4 color = CalcPointLight(light.Base, normal);
        float spotLightIntensity = (1.0 - (1.0 - spotFactor) / (1.0 - light.Cutoff));
        return color * spotLightIntensity;
    }
}

void main()
{
    vec3 normal = normalize(Normal);

    vec4 totalLight = CalcDirectionalLight(normal);
    
    for(int i = 0; i < u_NumPointLights; i++)
    {
        totalLight += CalcPointLight(u_PointLights[i], normal);
    }

    for(int i = 0; i < u_NumSpotLights; i++)
    {
        totalLight += CalcSpotLight(u_SpotLights[i], normal);
    }
    
    FragColor = u_Color * totalLight;
// check whether result is higher than some threshold, if so, output as bloom threshold color
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
	else
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}