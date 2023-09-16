#include <wipch.h>
#include "FrameBuffer.h"

#include <glew.h>
#include <Wiwa/utilities/Log.h>

namespace Wiwa {
	FrameBuffer::FrameBuffer()
	{
		m_Init = false;
	}

	FrameBuffer::~FrameBuffer()
	{
		if (m_Init) {
			//glDeleteTextures(1, &m_ColorBufferTexture);
			GL(DeleteRenderbuffers(1, &m_RBO));
			//glDeleteFramebuffers(1, &m_FBOs);

			for (int i = 0; i < m_FBOs.size(); i++)
			{
				GL(DeleteFramebuffers(1, &m_FBOs[i]));
			}
			for (int i = 0; i < m_ColorBuffers.size(); i++)
			{
				GL(DeleteFramebuffers(1, &m_ColorBuffers[i]));
			}
		}
	}

	void FrameBuffer::Init(int width, int height, int n_Textures, bool depth)
	{
		m_Init = true;		

		m_Width = width;
		m_Height = height;
		// FRAMEBUFFER
		m_FBOs.resize(1);
		GL(GenFramebuffers(1, &m_FBOs[0]));
		GL(BindFramebuffer(GL_FRAMEBUFFER, m_FBOs[0]));

		m_ColorBuffers.resize(n_Textures);
		//add 2 color buffer, one for normal rendering and the other for bloom
		GL(GenTextures(m_ColorBuffers.size(), &m_ColorBuffers[0]));
		for (unsigned int i = 0; i < m_ColorBuffers.size(); i++)
		{
	
			GL(BindTexture(GL_TEXTURE_2D, m_ColorBuffers[i]));
			GL(TexImage2D(
				GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL
			));
			GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
			// attach texture to framebuffer
			GL(FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_ColorBuffers[i], 0));
		}

		if (depth) {
			// Render buffer object for depth
			GL(GenRenderbuffers(1, &m_RBO));
			GL(BindRenderbuffer(GL_RENDERBUFFER, m_RBO));
			GL(RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
			GL(BindRenderbuffer(GL_RENDERBUFFER, 0));

			GL(FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO));
		}

		// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
		m_Atachtments.push_back(GL_COLOR_ATTACHMENT0);
		m_Atachtments.push_back(GL_COLOR_ATTACHMENT1);
		GL(DrawBuffers(m_Atachtments.size(),&m_Atachtments[0]));

		int fbstatus = GL(CheckFramebufferStatus(GL_FRAMEBUFFER));

		// Check framebuffer status
		if (fbstatus != GL_FRAMEBUFFER_COMPLETE)
		{
			WI_CORE_ASSERT_MSG("Framebuffer not completed")
		}
		else
			WI_CORE_INFO("Framebuffer completed");
	

		GL(BindFramebuffer(GL_FRAMEBUFFER, 0));
	}
	
	void FrameBuffer::Init(int width, int height, bool depth)
	{
		m_Init = true;		

		m_Width = width;
		m_Height = height;
		m_FBOs.resize(1);
		// FRAMEBUFFER
		GL(GenFramebuffers(1, &m_FBOs[0]));
		GL(BindFramebuffer(GL_FRAMEBUFFER, m_FBOs[0]));

		m_ColorBuffers.resize(1);
		// Color texture
		GL(GenTextures(1, &m_ColorBuffers[0]));
		GL(BindTexture(GL_TEXTURE_2D, m_ColorBuffers[0]));


		GL(TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
		GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

		GL(FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorBuffers[0], 0));
		
		if (depth) {
			// Render buffer object for depth
			GL(GenRenderbuffers(1, &m_RBO));
			GL(BindRenderbuffer(GL_RENDERBUFFER, m_RBO));
			GL(RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
			GL(BindRenderbuffer(GL_RENDERBUFFER, 0));

			GL(FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO));
		}

		int fbstatus = GL(CheckFramebufferStatus(GL_FRAMEBUFFER));

		// Check framebuffer status
		if (fbstatus != GL_FRAMEBUFFER_COMPLETE)
		{
			WI_CORE_ASSERT_MSG("Framebuffer not completed")
		}
		else
			WI_CORE_INFO("Framebuffer completed");
	

		GL(BindFramebuffer(GL_FRAMEBUFFER, 0));
	}

	void FrameBuffer::Bind(bool clear)
	{	
		GL(BindFramebuffer(GL_FRAMEBUFFER, m_FBOs[0]));
		//glEnable(GL_FRAMEBUFFER_SRGB);
		if (clear) {
			GL(ClearColor(0.0f, 0.0f, 0.0f, 0.0f));
			GL(Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
		}
	}


	void FrameBuffer::Unbind()
	{
		GL(BindFramebuffer(GL_FRAMEBUFFER, 0));
		//glDisable(GL_FRAMEBUFFER_SRGB);
	}

	void FrameBuffer::Clear()
	{
		GL(BindFramebuffer(GL_FRAMEBUFFER, m_FBOs[0]));
		GL(ClearColor(0.0f, 0.0f, 0.0f, 0.0f));
		GL(Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
		GL(BindFramebuffer(GL_FRAMEBUFFER, 0));
	}

	void FrameBuffer::Clear(const Vector4f& color)
	{
		GL(BindFramebuffer(GL_FRAMEBUFFER, m_FBOs[0]));
		GL(ClearColor(color.r, color.g, color.b, color.a));
		GL(Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
		GL(BindFramebuffer(GL_FRAMEBUFFER, 0));
	}

	inline std::vector<uint32_t> FrameBuffer::getColorBuffers()
	{
		return m_ColorBuffers;
	}
}