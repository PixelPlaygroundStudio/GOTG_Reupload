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
uniform vec2 u_SpriteSize; // size of each sprite in pixels
uniform int u_NumColumns; // number of columns in the spritesheet
uniform int u_NumRows; // number of rows in the spritesheet
uniform int u_NumFrames; // number of frames to animate
uniform float u_AnimationSpeed; // speed of the animation
uniform bool u_LoopAnimation; // whether the animation should loop or not
uniform bool u_PingPong; // whether the animation should ping-pong

void main()
{

   // Calculate the current frame based on the current time and animation speed
    float currentFrame = mod(u_Time * u_AnimationSpeed, float(u_NumFrames));
    
    // If the animation should ping-pong, reverse direction when we reach the end
    if (u_PingPong && int(currentFrame) == u_NumFrames - 1) {
        currentFrame = float(u_NumFrames) - currentFrame - 2.0;
    }
    
    // If the animation should loop, wrap around to the beginning when we reach the end
    if (u_LoopAnimation) {
        currentFrame = mod(currentFrame, float(u_NumFrames));
    }
    
    // Calculate the UV coordinates of the current frame in the spritesheet
    vec2 uv = vec2(
        mod(currentFrame, float(u_NumColumns)) / float(u_NumColumns),
        1.0 - (floor(currentFrame / float(u_NumColumns)) + 1.0) / float(u_NumRows)
    );
    
    // Calculate the pixel coordinates of the current sprite in the spritesheet
    vec2 pixelCoord = uv * u_SpriteSize;
    
    // Sample the texture at the pixel coordinates
    vec4 color = texture(u_Texture, pixelCoord);
    
    // Output the color
    FragColor = color;

    // check whether result is higher than some threshold, if so, output as bloom threshold color
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
	else
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}