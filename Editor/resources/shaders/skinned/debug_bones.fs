#version 330



in vec3 LocalPos;
in vec3 Normal;
in vec2 TexCoord;
flat in ivec4 BoneIDs;
in vec4 Weights;

out vec4 FragColor;

uniform int u_DisplayBoneIndex = 0;


void main()
{
    bool found = false;
    for(int i = 0; i< 4; i++ )
    {
        if(BoneIDs[i] == u_DisplayBoneIndex)
        {
             if(Weights[i] >= 0.7)
             {
                 FragColor = vec4(1.0,0.0,0.0,1.0)* Weights[i];
             }else if(Weights[i] >= 0.4 && Weights[i] <= 0.6){
                 FragColor = vec4(1.0,1.0,0.0,1.0)* Weights[i];
             }else if(Weights[i] >= 0.1){
                 FragColor = vec4(0.0,1.0,0.0,1.0)* Weights[i];
             }else{
                 FragColor =  vec4(0.0,0.0,1.0,1.0);
             }
             found = true;
             break;
        }
     }
     if(!found)
        FragColor =  vec4(0.0,0.0,1.0,1.0);
       
}