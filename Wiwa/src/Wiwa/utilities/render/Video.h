#pragma once

#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/containers/List.h>
#include <Wiwa/utilities/math/Vector2i.h>
#include <stdint.h>

#include <opencv2/core/core.hpp>

#include <opencv2/videoio.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <Wiwa/utilities/time/Time.h>

namespace Wiwa {
	class WI_API Video {
	private:
		uint32_t m_TextureId;
		Size2i m_ImageSize;
		cv::VideoCapture m_Cap;
		cv::Mat m_Frame;
		int m_FrameTime;
		double m_LastTime;
		bool m_IsPlaying;
		double m_CurrentTime;
		double m_ElapsedTime;
		unsigned char* m_VideoImage;
	public:
		Video();
		~Video();

		bool Init(std::string videoPath, byte* data);
		void Play();
		void Stop();
		void Replay();
		bool Update();

		uint32_t GetTextureId() { return m_TextureId; }

		int GetWidth() { return m_ImageSize.w; }
		int GetHeight() { return m_ImageSize.h; }
		Size2i GetSize() { return m_ImageSize; }

		bool HasFinished() { return m_Frame.empty(); }

		double GetFrameRate() { return Time::GetTargetFPS(); }
		unsigned char* cvMat2TexInput(cv::Mat& img);

		static void WriteVideo(const char* destination, Video* video);
	};
}