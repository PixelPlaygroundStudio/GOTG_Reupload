#pragma once

#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/containers/List.h>
#include <Wiwa/utilities/math/Vector2i.h>
#include <stdint.h>

namespace Wiwa {
	class WI_API Text {
	private:
		uint32_t m_TextureId;
		Size2i m_ImageSize;
	public:
		Text();
		~Text();

		bool Init(int width,int height, byte* data);
		bool InitWrapped(int width, int height, int lineWidth, int y, int descent, byte* data);

		uint32_t GetTextureId() { return m_TextureId; }

		int GetWidth() { return m_ImageSize.w; }
		int GetHeight() { return m_ImageSize.h; }
		Size2i GetSize() { return m_ImageSize; }
	};
}