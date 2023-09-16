#include <wipch.h>

#include "Text.h"

#include "../vendor/gli/gli/gli.hpp"
#include <glew.h>

namespace Wiwa {
	Text::Text() : m_TextureId(-1)
	{

	}

	Text::~Text()
	{
		if (m_TextureId != uint32_t(-1)) {
			GL(DeleteTextures(1, &m_TextureId));
		}
	}

	bool Text::Init(int width, int height, byte* data)
	{
		int w = width, h = height;

		GL(GenTextures(1, &m_TextureId));
		GL(BindTexture(GL_TEXTURE_2D, m_TextureId));
		GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED));
		GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED));
		GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED))	;
	
		GL(TexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			width,
			height,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			data));



		m_ImageSize.x = w;
		m_ImageSize.y = h;


		return true;
	}

	bool Text::InitWrapped(int width, int height, int lineWidth, int y, int descent, byte* data)
	{
		int w = width, h = height;

		GL(GenTextures(1, &m_TextureId));
		GL(BindTexture(GL_TEXTURE_2D, m_TextureId));
		GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED));
		GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED));
		GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED));

		GL(TexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			width,
			height,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			data));



		m_ImageSize.x = w;
		m_ImageSize.y = h;

		m_ImageSize.w = lineWidth;
		m_ImageSize.h = y + descent;

		return true;
	}
}