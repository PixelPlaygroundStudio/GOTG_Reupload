#include <wipch.h>

#include "Skybox.h"
#include "../vendor/stb/stb_image.h"
#include "glew.h"
#include <Wiwa/core/Resources.h>

Wiwa::Skybox::Skybox()
{
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    GL(GenVertexArrays(1, &VAO));
    GL(GenBuffers(1, &VBO));

    GL(BindVertexArray(VAO));
    GL(BindBuffer(GL_ARRAY_BUFFER, VBO));
    GL(BufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW));
    
    GL(EnableVertexAttribArray(0));
    GL(VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));

    GL(BindBuffer(GL_ARRAY_BUFFER, 0));
    GL(BindVertexArray(0));

    ResourceId skyboxShaderID = Resources::Load<Shader>("resources/shaders/skybox");
    Shader* shader = Resources::GetResourceById<Shader>(skyboxShaderID);
    
    shader->Compile("resources/shaders/skybox");
    m_Material = new Material(shader);

}

Wiwa::Skybox::~Skybox()
{
    GL(DeleteBuffers(1, &VBO));
    GL(DeleteVertexArrays(1, &VAO));
    GL(DeleteTextures(1, &m_TextureID));
}

void Wiwa::Skybox::LoadCubemap(const std::vector<const char*>& faces)
{
    unsigned int textureID;
    GL(GenTextures(1, &textureID));
    GL(BindTexture(GL_TEXTURE_CUBE_MAP, textureID));

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i], &width, &height, &nrChannels, STBI_rgb_alpha);
        if (data)
        {
            GL(TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data
            ));
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    GL(TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL(TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL(TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL(TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL(TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

    m_TextureID = textureID;
}

void Wiwa::Skybox::Render()
{
    GL(DepthMask(GL_FALSE));
    GL(BindVertexArray(VAO));
    GL(ActiveTexture(GL_TEXTURE0));
    GL(BindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID));
    GL(DrawArrays(GL_TRIANGLES, 0, 36));
    GL(BindVertexArray(0));
    GL(DepthMask(GL_TRUE));
}
