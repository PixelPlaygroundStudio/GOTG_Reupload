#pragma once
#include <Wiwa/core/Core.h>
#include <stdint.h>

namespace Wiwa
{
	class WI_API ShadowBuffer
	{
	private:
		uint32_t m_DBO = 0;
		uint32_t m_ShadowMap = 0;

		uint32_t m_ShadowWidth = 2048, m_ShadowHeight = 2048;
		bool m_Init;
	public:
		
		ShadowBuffer();
		~ShadowBuffer();
		
		void Init(uint32_t width = 2048, uint32_t height = 2048);

		void RegenShadowMap(uint32_t width, uint32_t height);
		void Bind(bool clear = true);
		void Unbind();

		void BindTexture();
		void UnbindTexture();

		void Clear();

		inline uint32_t GetWidth() { return m_ShadowWidth; }
		inline uint32_t GetHeight() { return m_ShadowHeight; }

		inline uint32_t GetShadowMap() { return m_ShadowMap; }
		inline uint32_t GetDepthBuffer() { return m_DBO; }
	};
}