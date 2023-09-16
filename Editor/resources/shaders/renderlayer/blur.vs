#version 330 core

layout (location = 0) in vec3 l_Pos;
layout (location = 1) in vec2 l_TexCoord;

out vec2 s_TexCoord;

void main()
{	
	gl_Position = vec4(l_Pos, 1.0);
	s_TexCoord = l_TexCoord;
}