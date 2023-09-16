#include <wipch.h>
#include "ShadowBuffer.h"

#include <glew.h>

Wiwa::ShadowBuffer::ShadowBuffer() 
	: m_Init(false)
{
}

Wiwa::ShadowBuffer::~ShadowBuffer()
{
	if (m_Init) {
		GL(DeleteTextures(1, &m_ShadowMap));
		GL(DeleteFramebuffers(1, &m_DBO));
	}
}

void Wiwa::ShadowBuffer::Init(uint32_t width, uint32_t height)
{
	m_Init = true;

	m_ShadowHeight = height;
	m_ShadowWidth = width;

	GL(GenFramebuffers(1, &m_DBO));

	GL(GenTextures(1, &m_ShadowMap));
	GL(BindTexture(GL_TEXTURE_2D, m_ShadowMap));
	GL(TexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_ShadowHeight, m_ShadowWidth, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
	GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
	GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GL(TexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));

	GL(BindFramebuffer(GL_FRAMEBUFFER, m_DBO));
	GL(FramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ShadowMap, 0));

	GL(DrawBuffer(GL_NONE));
	GL(ReadBuffer(GL_NONE));

	GL(BindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));

	int fbostatus = GL(CheckFramebufferStatus(GL_FRAMEBUFFER));

	if (fbostatus != GL_FRAMEBUFFER_COMPLETE)
	{
		WI_CORE_ASSERT_MSG("Shadow buffer not completed")
	}
	else
		WI_CORE_INFO("Shadow buffer completed");

	
}

void Wiwa::ShadowBuffer::RegenShadowMap(uint32_t width, uint32_t height)
{
	m_ShadowHeight = height;
	m_ShadowWidth = width;

	GL(DeleteTextures(1, &m_ShadowMap));

	GL(GenTextures(1, &m_ShadowMap));
	GL(BindTexture(GL_TEXTURE_2D, m_ShadowMap));
	GL(TexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_ShadowHeight, m_ShadowWidth, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
	GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
	GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GL(TexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));

	GL(BindFramebuffer(GL_FRAMEBUFFER, m_DBO));
	GL(FramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ShadowMap, 0));
	GL(DrawBuffer(GL_NONE));
	GL(ReadBuffer(GL_NONE));
	GL(Clear(GL_DEPTH_BUFFER_BIT));
	GL(BindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
}

void Wiwa::ShadowBuffer::Bind(bool clear)
{
	GL(Viewport(0, 0, m_ShadowWidth, m_ShadowHeight));
	GL(BindFramebuffer(GL_FRAMEBUFFER, m_DBO));
	if (clear)
	{
		GL(Clear(GL_DEPTH_BUFFER_BIT));
	}
	BindTexture();
}

void Wiwa::ShadowBuffer::Unbind()
{
	GL(ActiveTexture(GL_TEXTURE0));
	GL(BindTexture(GL_TEXTURE_2D, 0));
	GL(BindFramebuffer(GL_FRAMEBUFFER, 0));
}

void Wiwa::ShadowBuffer::BindTexture()
{
	GL(ActiveTexture(GL_TEXTURE0));
	GL(BindTexture(GL_TEXTURE_2D, m_ShadowMap));
}

void Wiwa::ShadowBuffer::UnbindTexture()
{
	GL(ActiveTexture(GL_TEXTURE0));
	GL(BindTexture(GL_TEXTURE_2D, 0));
}

void Wiwa::ShadowBuffer::Clear()
{
	GL(BindFramebuffer(GL_FRAMEBUFFER, m_DBO));
	GL(Clear(GL_DEPTH_BUFFER_BIT));
	GL(BindFramebuffer(GL_FRAMEBUFFER, 0));
}
