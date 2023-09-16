#version 330

out vec4 FragColor;


uniform sampler2D u_PositionTexture;
uniform sampler2D u_Texture;

float near = 0.1; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{             

    float minSeparation = 1.0;
    float maxSeparation = 3.0;
    float minDistance   = 0.5;
    float maxDistance   = 2.0;
    int   size          = 1;

    vec3 colorModifier  = vec3(0.324, 0.063, 0.099);

    vec2 texSize   = textureSize(u_Texture, 0).xy;
    vec2 fragCoord = gl_FragCoord.xy;
    vec2 texCoord  = fragCoord / texSize;

    vec4 position = texture(u_PositionTexture, texCoord);

    float depth = LinearizeDepth(gl_FragCoord.z) / far; 

    float separation = mix(maxSeparation, minSeparation, depth);
    

    float mx = 0.0;

    for (int i = -size; i <= size; ++i) {
        for (int j = -size; j <= size; ++j) {
        texCoord =
          ( fragCoord
          + (vec2(i, j) * separation)
          )
        / texSize;

        vec4 positionTemp =
         texture
          ( u_PositionTexture
          , texCoord
          );

        mx = max(mx, abs(position.y - positionTemp.y));
        }
    }
    
    float diff = smoothstep(minDistance, maxDistance, mx);
    
    vec3 lineColor = texture(u_Texture, texCoord).rgb * colorModifier;

    FragColor.rgb = mix(vec3(depth), lineColor, clamp(diff, 0.0, 1.0));
    FragColor.a   = 1.0;
   
  //  FragColor = vec4(vec3(depth), 1.0);
}