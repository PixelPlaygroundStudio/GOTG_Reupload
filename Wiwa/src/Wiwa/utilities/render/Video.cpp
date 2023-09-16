#include <wipch.h>
#include "Video.h"

#include <glew.h>
#include <gl/GL.h>
#include <GLFW/glfw3.h>
namespace Wiwa {
	Video::Video() : m_TextureId(-1), m_IsPlaying(true)
	{

	}

	Video::~Video()
	{
		if (m_TextureId != uint32_t(-1)) {
			GL(DeleteTextures(1, &m_TextureId));
		}
		m_Cap.release();
		m_Frame.release();
	}

	bool Video::Init(std::string videoPath, byte* data)
	{
		cv::VideoCapture cap(videoPath);
		m_Cap = cap;
		if (!m_Cap.isOpened())
		{

			WI_CORE_ERROR("Video file not loaded!");
			return false;

		}
		m_Cap >> m_Frame;
		int videoWidth = m_Frame.cols;
		int videoHeight = m_Frame.rows;
		m_VideoImage = cvMat2TexInput(m_Frame);
		GL(GenTextures(1, &m_TextureId));
		GL(BindTexture(GL_TEXTURE_2D, m_TextureId));
		GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

		GL(TexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGB,
			videoWidth,
			videoHeight,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			m_VideoImage));

		m_ImageSize.w = videoWidth;
		m_ImageSize.h = videoHeight;

		m_FrameTime = int(1000 / GetFrameRate()); 
		m_LastTime = Time::GetDeltaTimeSeconds();
		
		return true;
	}

	void Video::Play()
	{
		m_IsPlaying = true;
	}

	void Video::Stop()
	{
		m_IsPlaying = false;
	}

	void Video::Replay()
	{
		m_Cap.set(cv::CAP_PROP_POS_FRAMES, 0);
	}

	bool Video::Update()
	{
		if (!m_IsPlaying)
			return false;
		m_CurrentTime = glfwGetTime(); 
		m_ElapsedTime = (m_CurrentTime - m_LastTime) * 1000; 
		if (m_ElapsedTime >= m_FrameTime) {

			m_Cap >> m_Frame;

			if (m_Frame.empty())
			{
				return false;
			}
			m_VideoImage = cvMat2TexInput(m_Frame);

			if (m_VideoImage)
			{
				GL(DeleteTextures(1, &m_TextureId));

				GL(GenTextures(1, &m_TextureId));
				GL(BindTexture(GL_TEXTURE_2D, m_TextureId));
				GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
				GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
				GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
				GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
				GL(TexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_ImageSize.w, m_ImageSize.h, 0, GL_RGB, GL_UNSIGNED_BYTE, m_VideoImage));

			}

			m_LastTime = m_CurrentTime;
		}
		
		return true;
	}

	unsigned char* Video::cvMat2TexInput(cv::Mat& img)
	{
		cvtColor(img, img, cv::COLOR_BGR2RGB);
		cv::rotate(img, img, cv::ROTATE_90_CLOCKWISE);
		transpose(img, img);
		flip(img, img, 0);
		return img.data;
	}
	void Video::WriteVideo(const char* destination, Video* video)
	{
		/*int fourcc = static_cast<int>(video->m_Cap.get(cv::CAP_PROP_FOURCC));
		cv::VideoWriter oVideoWriter(destination,fourcc,
			(int)video->GetFrameRate(), cv::Size(video->GetSize().w,video->GetSize().h), true);

		If the VideoWriter object is not initialized successfully, exit the program
		if (oVideoWriter.isOpened() == false)
		{
			WI_CORE_ERROR("Cannot save the video to a file");
		}

		cv::Mat frame;
		for (int i = 0; i < video->m_Cap.get(cv::CAP_PROP_FRAME_COUNT); i ++)
		{
			video->m_Cap >> frame;
			oVideoWriter.write(frame);
		}*/
	}
}