#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;
layout (location = 3) in ivec4 aBoneIDs;
layout (location = 4) in vec4 aWeights;

out vec3 Normal;
out vec3 LocalPos;
out vec2 TexCoord;
flat out ivec4 BoneIDs;
out vec4 Weights;

uniform mat4 u_Model;
uniform mat4 u_Proj;
uniform mat4 u_View;

void main()
{
	gl_Position = u_Proj * u_View * u_Model * vec4(aPos, 1.0);
	TexCoord = aTex;
	Normal = aNormal;
	LocalPos = aPos;
	BoneIDs = aBoneIDs;
    Weights = aWeights;
}
