#pragma once


namespace Wiwa {
	class BlurBuffer
	{
	public:
		BlurBuffer();
		~BlurBuffer();

		void Init(int _width, int _height);

		void Bind(bool clear);

		void UnBind();

		void BindTexture();

		void Clear();


		uint32_t getFBO() { return m_FBO; }
		uint32_t getColorTexture() { return m_ColorTexture;	}

		int getWidth() { return m_Width; }
		int getHeight() { return m_Height; }

	private:
		int m_Height, m_Width;
		uint32_t m_FBO = 0;
		uint32_t m_ColorTexture = 0;
		bool m_Init;
	};

}


