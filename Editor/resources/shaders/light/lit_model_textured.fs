#version 330 core

const int MAX_POINT_LIGHTS = 32;
const int MAX_SPOT_LIGHTS = 32;

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 LocalPos;

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

uniform vec4 u_MatAmbientColor;
uniform vec4 u_MatDiffuseColor;
uniform vec4 u_MatSpecularColor;

uniform DirectionalLight u_DirectionalLight;
uniform int u_NumPointLights = 0;
uniform PointLight[MAX_POINT_LIGHTS] u_PointLights;
uniform int u_NumSpotLights = 0;
uniform SpotLight[MAX_SPOT_LIGHTS] u_SpotLights;

uniform sampler2D u_ShadowMap;
uniform sampler2D u_Texture;
uniform float u_SpecularValue;
uniform vec3 u_CameraPosition;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    
}

vec4 CalcLightInternal(BaseLight light, vec3 direction, vec3 normal)
{
    vec4 ambientColor = vec4(light.Color, 1.0f) * light.AmbientIntensity * u_MatAmbientColor;

    float diffuseFactor = dot(normal, -direction);

    vec4 diffuseColor = vec4(0, 0, 0, 0);
    vec4 specularColor = vec4(0, 0, 0, 0);

    if(diffuseFactor > 0) 
    {
        diffuseColor = vec4(light.Color, 1.0f) * light.DiffuseIntensity * u_MatDiffuseColor * diffuseFactor;

        vec3 pixelToCamera = normalize(u_CameraPosition - LocalPos);
        vec3 lightReflect = normalize(reflect(direction, normal));
        float specularFactor = dot(pixelToCamera, lightReflect);

        if(specularFactor > 0)
        {
            specularFactor = pow(specularFactor, u_SpecularValue);
            specularColor = vec4(light.Color, 1.0f) * light.DiffuseIntensity * u_MatSpecularColor * specularFactor;
        }
    }

    return ambientColor + diffuseColor + specularColor;
}

vec4 CalcDirectionalLight(vec3 normal)
{
    return CalcLightInternal(u_DirectionalLight.Base, u_DirectionalLight.Direction, normal);
}

vec4 CalcPointLight(PointLight light, vec3 normal)
{
    vec3 lightDirection = LocalPos - light.LocalPos;
    float distance = length(lightDirection);
    lightDirection = normalize(-lightDirection);

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
    else 
    {
        return vec4(0, 0, 0, 0);
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

    FragColor = texture2D(u_Texture, TexCoord.xy) *
                totalLight;
}