#version 330 core
out vec4 FragColor;

in vec2 s_TexCoord;

uniform sampler2D u_Image;
uniform bool u_Horizontal;
uniform float u_Weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main()
{             
     vec2 tex_offset = 1.0 / textureSize(u_Image, 0); // gets size of single texel
      vec3 result = texture(u_Image, s_TexCoord).rgb * u_Weight[0];
      if(u_Horizontal)
      {
          for(int i = 1; i < 5; ++i)
          {
             result += texture(u_Image, s_TexCoord + vec2(tex_offset.x * i, 0.0)).rgb * u_Weight[i];
             result += texture(u_Image, s_TexCoord - vec2(tex_offset.x * i, 0.0)).rgb * u_Weight[i];
          }
      }
      else
      {
          for(int i = 1; i < 5; ++i)
          {
              result += texture(u_Image, s_TexCoord + vec2(0.0, tex_offset.y * i)).rgb * u_Weight[i];
              result += texture(u_Image, s_TexCoord - vec2(0.0, tex_offset.y * i)).rgb * u_Weight[i];
         }
      }
    FragColor = vec4(result, 0.5f);
}