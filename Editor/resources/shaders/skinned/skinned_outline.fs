#version 330 core

// Texture sampler for the model texture
uniform sampler2D u_Texture;
// Position of the camera
uniform vec3 u_CameraPosition;
// Outline percentage
uniform vec2 u_OutlineSmoothRange;
// Outline color
uniform vec4 u_OutlineColor;

// Input from the vertex shader
in vec3 LocalPos;
in vec2 TexCoord;
in vec3 Normal;
// Output color of the fragment
out vec4 FragColor;

void main()
{

  //direction vertex to cam
  vec3 dir = u_CameraPosition - LocalPos;

  float dotEyeNormal = dot(normalize(dir),normalize(Normal));
  dotEyeNormal =abs(dotEyeNormal);


  FragColor = mix(u_OutlineColor, texture(u_Texture,TexCoord), smoothstep(u_OutlineSmoothRange.x,u_OutlineSmoothRange.y,dotEyeNormal));

}
