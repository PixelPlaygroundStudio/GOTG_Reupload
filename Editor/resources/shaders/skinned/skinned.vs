#version 330
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_separate_shader_objects : enable
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;
layout (location = 3) in ivec4 aBoneIDs;
layout (location = 4) in vec4 aWeights;

layout (location = 9) out vec3 Normal;
layout (location = 10) out vec3 LocalPos;
layout (location = 11) out vec2 TexCoord;
layout (location = 12) flat out ivec4 BoneIDs;
layout (location = 13) out vec4 Weights;

layout (location = 5) uniform mat4 u_Model;
layout (location = 6) uniform mat4 u_Proj;
layout (location = 7) uniform mat4 u_View;
layout (location = 8) uniform mat4 u_Bones[MAX_BONES];

void main()
{
    mat4 BoneTransform = u_Bones[aBoneIDs[0]] * aWeights[0];
    BoneTransform     += u_Bones[aBoneIDs[1]] * aWeights[1];
    BoneTransform     += u_Bones[aBoneIDs[2]] * aWeights[2];
    BoneTransform     += u_Bones[aBoneIDs[3]] * aWeights[3];

    gl_Position = u_Proj * u_View * u_Model  * BoneTransform * vec4(aPos, 1.0);
    TexCoord = aTex;
    Normal = aNormal;
    LocalPos = aPos;
    BoneIDs = aBoneIDs;
    Weights = aWeights;
}