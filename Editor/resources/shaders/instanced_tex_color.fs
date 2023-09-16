#version 330 core

uniform sampler2D u_Textures[32];

in vec4 f_Color;
in vec2 f_TexCoord;
in float f_TexID;
in float f_Active;

out vec4 out_color;

void main() {
	if(f_Active < 0.5) discard;
	vec2 texCoordFlip = f_TexCoord;
	int index = int(f_TexID);
	
	switch(index)
	{
		case 0:
		out_color = texture2D(u_Textures[0], texCoordFlip) * f_Color;
		break;
		case 1:
		out_color = texture2D(u_Textures[1], texCoordFlip) * f_Color;
		break;
		case 2:
		out_color = texture2D(u_Textures[2], texCoordFlip) * f_Color;
		break;
		case 3:
		out_color = texture2D(u_Textures[3], texCoordFlip) * f_Color;
		break;
		case 4:
		out_color = texture2D(u_Textures[4], texCoordFlip) * f_Color;
		break;
		case 5:
		out_color = texture2D(u_Textures[5], texCoordFlip) * f_Color;
		break;
		case 6:
		out_color = texture2D(u_Textures[6], texCoordFlip) * f_Color;
		break;
		case 7:
		out_color = texture2D(u_Textures[7], texCoordFlip) * f_Color;
		break;
		case 8:
		out_color = texture2D(u_Textures[8], texCoordFlip) * f_Color;
		break;
		case 9:
		out_color = texture2D(u_Textures[9], texCoordFlip) * f_Color;
		break;
		case 10:
		out_color = texture2D(u_Textures[10], texCoordFlip) * f_Color;
		break;
		case 11:
		out_color = texture2D(u_Textures[11], texCoordFlip) * f_Color;
		break;
		case 12:
		out_color = texture2D(u_Textures[12], texCoordFlip) * f_Color;
		break;
		case 13:
		out_color = texture2D(u_Textures[13], texCoordFlip) * f_Color;
		break;
		case 14:
		out_color = texture2D(u_Textures[14], texCoordFlip) * f_Color;
		break;
		case 15:
		out_color = texture2D(u_Textures[15], texCoordFlip) * f_Color;
		break;
		case 16:
		out_color = texture2D(u_Textures[16], texCoordFlip) * f_Color;
		break;
		case 17:
		out_color = texture2D(u_Textures[17], texCoordFlip) * f_Color;
		break;
		case 18:
		out_color = texture2D(u_Textures[18], texCoordFlip) * f_Color;
		break;
		case 19:
		out_color = texture2D(u_Textures[19], texCoordFlip) * f_Color;
		break;
		case 20:
		out_color = texture2D(u_Textures[20], texCoordFlip) * f_Color;
		break;
		case 21:
		out_color = texture2D(u_Textures[21], texCoordFlip) * f_Color;
		break;
		case 22:
		out_color = texture2D(u_Textures[22], texCoordFlip) * f_Color;
		break;
		case 23:
		out_color = texture2D(u_Textures[23], texCoordFlip) * f_Color;
		break;
		case 24:
		out_color = texture2D(u_Textures[24], texCoordFlip) * f_Color;
		break;
		case 25:
		out_color = texture2D(u_Textures[25], texCoordFlip) * f_Color;
		break;
		case 26:
		out_color = texture2D(u_Textures[26], texCoordFlip) * f_Color;
		break;
		case 27:
		out_color = texture2D(u_Textures[27], texCoordFlip) * f_Color;
		break;
		case 28:
		out_color = texture2D(u_Textures[28], texCoordFlip) * f_Color;
		break;
		case 29:
		out_color = texture2D(u_Textures[29], texCoordFlip) * f_Color;
		break;
		case 30:
		out_color = texture2D(u_Textures[30], texCoordFlip) * f_Color;
		break;
		case 31:
		out_color = texture2D(u_Textures[31], texCoordFlip) * f_Color;
		break;
	}

	if(out_color.a < 0.1)
		discard;
}