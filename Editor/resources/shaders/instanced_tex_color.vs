#version 330 core

// Vertex data
layout (location = 0) in vec3 l_VPos;
layout (location = 1) in vec2 l_TexCoord;
// Instanced data
layout (location = 2) in vec2 l_PosOffset;
layout (location = 3) in vec2 l_Scale;
layout (location = 4) in vec4 l_Color;
layout (location = 5) in float l_TexID;
layout (location = 6) in vec2 l_TexClip[4]; // from loc 6 to 9
layout (location = 10) in float l_Active;
layout (location = 11) in float l_Rotation;
layout (location = 12) in float l_Priority;

//uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;

out vec4 f_Color;
out vec2 f_TexCoord;
out float f_TexID;
out float f_Active;

mat4 rotationZ( in float angle ) {
    return mat4(cos(angle), -sin(angle), 0.0, 0.0,
                sin(angle), cos(angle), 0.0, 0.0,
				0.0, 0.0, 1.0, 0.0,
				0.0, 0.0, 0.0, 1.0);
}

void main()
{	
	if(l_Active > 0.5){

		mat4 model = rotationZ(radians(l_Rotation));
		vec4 vpos = vec4(l_VPos, 1.0);
	
		//Scale		
		vpos.xy = vpos.xy * l_Scale.xy;
		//Rotate
		vpos = model * vpos;
		// Translate
		vpos.xy += l_PosOffset;
		
		vpos.z = l_Priority;
		// Out position
		gl_Position = u_Proj * u_View * vpos;
		
		f_Color = l_Color;
		f_TexCoord = l_TexClip[gl_VertexID];
		f_TexID = l_TexID;
	}
	f_Active = l_Active;
}