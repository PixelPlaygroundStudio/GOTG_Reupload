#include "wipch.h"
#include "Uniform.h"
#include <Wiwa/utilities/time/Time.h>
#include <glew.h>

#include <glm/glm.hpp>

namespace Wiwa {
	//Sampler2D x : OpenGL id y: ResourceId to retrieve the path when saving
	void Uniform::sendToShader(uint16_t shaderProgram, int& textureId)
	{
		if (m_Type == Wiwa::UniformType::Invalid) {
			//WI_CRITICAL("Invalid uniform [{}]", name.c_str());
			return;
		}

		if (m_UniformID == -1)
			m_UniformID = glGetUniformLocation(shaderProgram, name.c_str());

		if (m_UniformID == -1) {
			//WI_CRITICAL("Uniform [{}] doesn't exist in program [{}]", name.c_str(), shaderProgram);
			return;
		}

		switch (m_Type)
		{
		case Wiwa::UniformType::Bool:
			GL(Uniform1i(m_UniformID, *(GLint*)m_Data));
			break;
		case Wiwa::UniformType::Int:
			GL(Uniform1i(m_UniformID, *(GLint*)m_Data));
			break;
		case Wiwa::UniformType::Uint:
			GL(Uniform1ui(m_UniformID, *(GLint*)m_Data));
			break;
		case Wiwa::UniformType::Float:
		{
			GL(Uniform1f(m_UniformID, *(GLfloat*)m_Data));
		}break;
		case Wiwa::UniformType::fVec2:
			GL(Uniform2fv(m_UniformID, 1, (GLfloat*)m_Data));
			break;
		case Wiwa::UniformType::fVec3:
			GL(Uniform3fv(m_UniformID, 1, (GLfloat*)(m_Data)));
			break;
		case Wiwa::UniformType::fVec4:
		{
			GL(Uniform4fv(m_UniformID, 1, (GLfloat*)m_Data));
		}break;
		case Wiwa::UniformType::Mat2:
			GL(UniformMatrix2fv(m_UniformID, 1, GL_FALSE, (GLfloat*)(m_Data)));
			break;
		case Wiwa::UniformType::Mat3:
			GL(UniformMatrix3fv(m_UniformID, 1, GL_FALSE, (GLfloat*)(m_Data)));
			break;
		case Wiwa::UniformType::Mat4:
			GL(UniformMatrix4fv(m_UniformID, 1, GL_FALSE, (GLfloat*)(m_Data)));
			break;
		case Wiwa::UniformType::Time:
		{
			GL(Uniform1f(m_UniformID, Wiwa::Time::GetRealTimeSinceStartup()));
			setData(Wiwa::Time::GetRealTimeSinceStartup(), m_Type);
		}break;
		case Wiwa::UniformType::Sampler2D:
		{
			if (((SamplerData*)m_Data)->tex_id > 0)
			{
				GL(ActiveTexture(textureId));
				GL(BindTexture(GL_TEXTURE_2D, ((SamplerData*)m_Data)->tex_id));
				GL(Uniform1i(m_UniformID, textureId - GL_TEXTURE0));
				textureId++;
			}
		}break;
		default:
			break;
		}
	}

	void Uniform::setEmptyData()
	{
		switch (m_Type)
		{
		case Wiwa::UniformType::Bool:
			setData(false, m_Type);
			break;
		case Wiwa::UniformType::Int:
			setData(0, m_Type);
			break;
		case Wiwa::UniformType::Uint:
			setData(0, m_Type);
			break;
		case Wiwa::UniformType::Float:
			setData(1.0f, m_Type);
			break;
		case Wiwa::UniformType::fVec2:
			setData(glm::vec2(), m_Type);
			break;
		case Wiwa::UniformType::fVec3:
			setData(glm::vec3(0.0f), m_Type);
			break;
		case Wiwa::UniformType::fVec4:
			setData(glm::vec4(1.0f), m_Type);
			break;
		case Wiwa::UniformType::Mat2:
			setData(glm::mat2(), m_Type);
			break;
		case Wiwa::UniformType::Mat3:
			setData(glm::mat3(), m_Type);
			break;
		case Wiwa::UniformType::Mat4:
			setData(glm::mat4(), m_Type);
			break;
		case Wiwa::UniformType::Time:
			setData(0.0f, m_Type);
			break;
		case Wiwa::UniformType::Sampler2D:
		{
			SamplerData data;
			data.tex_id = -1;
			data.resource_id = -1;
			setData(data, m_Type);
		}break;
		default:
			break;
		}
	}

}