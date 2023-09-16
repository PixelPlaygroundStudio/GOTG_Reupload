#pragma once

#include <Wiwa/core/Core.h>
#include <stdint.h>
#include <Wiwa/utilities/math/Vector3f.h>
#include <Wiwa/utilities/math/Vector4f.h>
#include <Wiwa/utilities/containers/List.h>
#include <glm/glm.hpp>

namespace Wiwa {
	
	class WI_API FrameBuffer {
	private:
		uint32_t m_RBO;
		int m_Width, m_Height;

		bool m_Init;
		std::vector<uint32_t> m_FBOs;
		std::vector<uint32_t> m_ColorBuffers;
		std::vector<uint32_t> m_Atachtments;
		
	public:
		FrameBuffer();
		~FrameBuffer();

		void Init(int width, int height, bool depth=true);
		void Init(int width, int height, int n_Textures, bool depth = true);

		void Bind(bool clear=true);
		void Unbind();


		void Clear();
		void Clear(const Vector4f& color);

		inline int getWidth() { return m_Width; }
		inline int getHeight() { return m_Height; }

		inline uint32_t getFBO() { return m_FBOs.front(); }
		inline std::vector<uint32_t> getFBOs() { return m_FBOs; }
		inline uint32_t getColorBufferTexture() { return m_ColorBuffers.front(); }
		inline std::vector<uint32_t> getColorBuffers();
		inline uint32_t getDepthBufferTexture() { return m_RBO; }
		
	};
}