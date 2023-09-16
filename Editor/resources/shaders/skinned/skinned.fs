#version 330
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_separate_shader_objects : enable
layout (location = 1) in vec3 LocalPos;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec2 TexCoord;
layout (location = 4) flat in ivec4 BoneIDs;
layout (location = 5) in vec4 Weights;

out vec4 FragColor;
void main()
{
        FragColor =  vec4(0.0,0.0,1.0,1.0);
}