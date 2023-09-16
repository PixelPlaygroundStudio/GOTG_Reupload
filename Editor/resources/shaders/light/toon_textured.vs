#version 330

// Positions/Coordinates
layout (location = 0) in vec3 aPos;
// Normals
layout (location = 1) in vec3 aNormal;
// Texture Coordinates
layout (location = 2) in vec2 aTex;

out vec3 Normal;
out vec3 LocalPos;
out vec2 TexCoord;
out vec4 LightSpacePos;

uniform mat4 u_Model;
uniform mat4 u_Proj;
uniform mat4 u_View;
uniform mat4 u_LightMVP;

void main()
{
	gl_Position = u_Proj * u_View * u_Model * vec4(aPos, 1.0);
	TexCoord = aTex;
	Normal = mat3(transpose(inverse(u_Model))) * aNormal;
	LocalPos = vec3(u_Model * vec4(aPos, 1.0f));

	LightSpacePos = u_LightMVP * vec4(LocalPos, 1.0);
}

