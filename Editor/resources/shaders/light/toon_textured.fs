#version 330 core
const int MAX_POINT_LIGHTS = 32;
const int MAX_SPOT_LIGHTS = 32;
out vec4 FragColor;

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

uniform vec4 u_MatAmbientColor;
uniform vec4 u_MatDiffuseColor;
uniform vec4 u_MatSpecularColor;
uniform int u_ToonLevels = 4;
uniform float u_RimLightPower = 4.0;

float ToonScaleFactor = 1.0f / u_ToonLevels;

uniform DirectionalLight u_DirectionalLight;
uniform int u_NumPointLights;
uniform PointLight[MAX_POINT_LIGHTS] u_PointLights;
uniform int u_NumSpotLights;
uniform SpotLight[MAX_SPOT_LIGHTS] u_SpotLights;

uniform sampler2D u_ShadowMap;
uniform sampler2D u_Texture;
uniform float u_SpecularValue;
uniform vec3 u_CameraPosition;
uniform vec3 u_LightPos;
uniform int u_RecieveShadows;

float CalcShadowFactor(vec3 position,vec3 direction)
{
     vec3 projCoords = LightSpacePos.xyz / LightSpacePos.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_ShadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(position - LocalPos);
    float bias = 0.005;
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

   if(projCoords.z > 1.0)
        shadow = 0.0;
    
    return shadow;
}

float CalcRimLightFactor(vec3 pixelToCamera, vec3 normal)
{
    float rimFactor = dot(pixelToCamera, normal);
    rimFactor = 1.0 - rimFactor;
    rimFactor = max(0.0, rimFactor);
    rimFactor = pow(rimFactor, u_RimLightPower);
    return rimFactor;
}

vec4 CalcLightInternal(BaseLight light, vec3 direction, vec3 normal, float shadowFactor)
{
    vec3 color = texture2D(u_Texture, TexCoord.xy).rgb;

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

    return (ambientColor + (1 - shadowFactor)  * (diffuseColor + specularColor + rimColor)) * vec4(color, 1.0);
}

vec4 CalcDirectionalLight(vec3 normal)
{
    float shadowFactor = 0;
    switch(u_RecieveShadows)
    {
    case 0:
    {
        shadowFactor = 0;
    }
    break;
    case 1:
    {
        shadowFactor = CalcShadowFactor(u_LightPos, u_DirectionalLight.Direction);
    }
    break;
    default:
    break;
    }

    return CalcLightInternal(u_DirectionalLight.Base, u_DirectionalLight.Direction, normal, shadowFactor);
}

vec4 CalcPointLight(PointLight light, vec3 normal)
{
    float shadowFactor = 1.0;

    vec3 lightDirection = LocalPos - light.LocalPos;
    float distance = length(lightDirection);
    lightDirection = normalize(lightDirection);

    vec4 color = CalcLightInternal(light.Base, lightDirection, normal, shadowFactor);

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

    FragColor = totalLight;
}