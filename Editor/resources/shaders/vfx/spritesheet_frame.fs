#version 330


layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 LocalPos;

//====================================
// all vfx must have this unifroms
//lifetime form 1 to 0,  100% normalized
uniform float u_LifeTime;
uniform float u_Time;
uniform vec4 u_Color;
//=====================
uniform sampler2D u_Texture;
uniform vec2 u_Position;
uniform float u_Zoom;
// uniform vec2 u_CellSize;
// uniform int u_Index;


void main()
{

    // int spritesheetW = int(u_SpritesheetSize.x);
    // int spritesheetH = int(u_SpritesheetSize.y);

    // int cellW = int(u_CellSize.x);
    // int cellH = int(u_CellSize.y);

    // int spriteColumn = u_Index % spritesheetW;
    // int spriteRow = u_Index / spritesheetW;

    // vec2 uv = vec2(
    //     float(spriteColumn * cellW) / float(spritesheetW * cellW),
    //     float(spriteRow * cellH) / float(spritesheetH * cellH)
    // );


    float zoom = 0;

    if (u_Zoom != 0)
    {
        zoom = 1.0f / u_Zoom;
    }

    vec2 uv_;
    uv_.x = TexCoord.x * zoom + u_Position.x * 0.01f;
    uv_.y = TexCoord.y * zoom + u_Position.y * 0.01f;

    FragColor = texture2D(u_Texture, uv_) * u_Color;

    // check whether result is higher than some threshold, if so, output as bloom threshold color
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
	else
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}