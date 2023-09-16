#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

layout (location = 0) in vec3 l_Pos;
layout (location = 1) in vec2 l_TexCoord;

out vec2 s_TexCoord;

layout(location = 2)uniform mat4 u_Model;
layout(location = 3)uniform mat4 u_View;
layout(location = 4)uniform mat4 u_Proj;

void main()
{	
	gl_Position = u_Proj * u_View * u_Model * vec4(l_Pos, 1.0);
	s_TexCoord = l_TexCoord;
}