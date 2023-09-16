#version 330

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;
layout (location = 3) in ivec4 aBoneIDs;
layout (location = 4) in vec4 aWeights;

out vec3 Normal;
out vec3 LocalPos;
out vec2 TexCoord;
out vec4 LightSpacePos;

uniform mat4 u_Model;
uniform mat4 u_Proj;
uniform mat4 u_View;
uniform mat4 u_LightMVP;
uniform mat4 u_Bones[MAX_BONES];

void main()
{
    mat4 BoneTransform = u_Bones[aBoneIDs[0]] * aWeights[0];
    BoneTransform     += u_Bones[aBoneIDs[1]] * aWeights[1];
    BoneTransform     += u_Bones[aBoneIDs[2]] * aWeights[2];
    BoneTransform     += u_Bones[aBoneIDs[3]] * aWeights[3];

    gl_Position = u_Proj * u_View * u_Model  * BoneTransform * vec4(aPos, 1.0);
	TexCoord = aTex;
    Normal = (u_Model* BoneTransform * vec4(aNormal, 0)).xyz;
	LocalPos = vec3(u_Model * vec4(aPos, 1.0f));

	LightSpacePos = u_LightMVP * vec4(LocalPos, 1.0);
}

