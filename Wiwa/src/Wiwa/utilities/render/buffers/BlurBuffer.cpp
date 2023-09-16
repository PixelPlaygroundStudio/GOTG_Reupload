#include "wipch.h"
#include "BlurBuffer.h"

#include <glew.h>

namespace Wiwa {
	BlurBuffer::BlurBuffer()
	{
	}
	BlurBuffer::~BlurBuffer()
	{
		GL(DeleteFramebuffers(1, &m_FBO));
		GL(DeleteTextures(2, &m_ColorTexture));
	}
	void BlurBuffer::Init(int width, int height)
	{
		m_Init = true;

		m_Width = width;
		m_Height = height;
		GL(GenFramebuffers(1, &m_FBO));
		GL(GenTextures(2, &m_ColorTexture));
		for (unsigned int i = 0; i < 2; i++)
		{
			GL(BindFramebuffer(GL_FRAMEBUFFER, m_FBO));
			GL(BindTexture(GL_TEXTURE_2D, m_ColorTexture));
			GL(TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
			GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
			GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
			GL(FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorTexture, 0));

			int fbstatus = GL(CheckFramebufferStatus(GL_FRAMEBUFFER));

			// also check if framebuffers are complete (no need for depth buffer)
			if (fbstatus != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "Framebuffer not complete!" << std::endl;
		}

		GL(BindFramebuffer(GL_FRAMEBUFFER, 0));
	}
	void BlurBuffer::Bind(bool clear)
	{
		GL(Viewport(0, 0, m_Width, m_Height));
		GL(BindFramebuffer(GL_FRAMEBUFFER, m_FBO));
		if (clear)
		{
			GL(Clear(GL_DEPTH_BUFFER_BIT));
		}
	}
	void BlurBuffer::UnBind()
	{
		GL(BindFramebuffer(GL_FRAMEBUFFER, 0));
	}
	void BlurBuffer::BindTexture()
	{
		GL(ActiveTexture(GL_TEXTURE0));
		GL(BindTexture(GL_TEXTURE_2D, m_ColorTexture));
	}
	void BlurBuffer::Clear()
	{
		GL(BindFramebuffer(GL_FRAMEBUFFER, m_FBO));
		GL(Clear(GL_DEPTH_BUFFER_BIT));
		GL(BindFramebuffer(GL_FRAMEBUFFER, 0));
	}
}