#pragma once

namespace Wiwa {
	enum class UniformType
	{
		Bool = 0,
		Int,
		Uint,
		Float,
		fVec2,
		fVec3,
		fVec4,
		Mat2,
		Mat3,
		Mat4,
		Time,
		Sampler2D,
		Invalid
	};
	//struct SamplerData
	//{
	//	uint32_t textureId;
	//	std::string path;

	//	SamplerData(){}
	//	SamplerData(uint32_t id, const char* _path)
	//		: textureId(id), path(_path){}
	//};
	typedef unsigned char byte;
	class Uniform
	{
	public:

		struct SamplerData {
			size_t tex_id;
			size_t resource_id;
			char tex_path[256];
		};

		Uniform()
			: m_Type(UniformType::Invalid), m_Data(nullptr), m_UniformID(-1)
		{}


		Uniform(std::string name, UniformType type)
			: m_Type(type), m_Data(nullptr), name(name), m_UniformID(-1)
		{
			setEmptyData();
		}
		~Uniform()
		{
			if (m_Data)
			{
				delete[] m_Data;
				m_Data = nullptr;
			}
		}
		void sendToShader(const uint16_t shaderProgram, int& textureId);

		template <class T>
		void setData(const T& value, const UniformType type)
		{
			setType(type);

			//If it's already a value we need to free the data
			if (m_Data)
				delete[] m_Data;

			//Reserving memory and copy the value
			m_DataSize = sizeof(T);
			m_Data = new byte[m_DataSize];
			memcpy(m_Data, &value, sizeof(T));
		}

		void setEmptyData();

		template <class T>
		void getData(T* data)
		{
			data = (T*)this->m_Data;
		}
		template <class T>
		T getData()
		{
			return *(T*)m_Data;
		}
		template <class T>
		T* getPtrData()
		{
			return (T*)m_Data;
		}
		inline UniformType getType() const { return m_Type; }
		inline void setType(UniformType val) { m_Type = val; }

		Uniform(Uniform const& uniform)
		{
			m_DataSize = uniform.m_DataSize;
			m_Data = new byte[m_DataSize];
			memcpy(m_Data, uniform.m_Data, m_DataSize);

			m_Type = uniform.m_Type;
			m_UniformID = uniform.m_UniformID;
			name = uniform.name;
		}
		Uniform& operator=(Uniform const& uniform)
		{
			if (this == &uniform) return *this;
			byte* tmp = new byte[uniform.m_DataSize];
			memcpy(tmp, uniform.m_Data, m_DataSize);
			delete[] m_Data;
			m_Data = tmp;
			m_DataSize = uniform.m_DataSize;
			m_Type = uniform.m_Type;
			m_UniformID = uniform.m_UniformID;
			name = uniform.name;
			return *this;
		}


		Uniform(Uniform&& rhs) noexcept
			: m_Data(rhs.m_Data), m_DataSize(rhs.m_DataSize), m_Type(rhs.m_Type), m_UniformID(rhs.m_UniformID), name(rhs.name)
		{
			rhs.m_Data = nullptr;
		}
		Uniform& operator&&(Uniform&& rhs) {
			if (this != &rhs)
			{
				delete m_Data;
				m_Data = rhs.m_Data;
				rhs.m_Data = nullptr;
				m_DataSize = rhs.m_DataSize;
				m_Type = rhs.m_Type;
				m_UniformID = rhs.m_UniformID;
				name = rhs.name;
			}
			return *this;
		}
	private:
		UniformType m_Type;
		byte* m_Data = nullptr;
		size_t m_DataSize = 0;
		int m_UniformID;
	public:
		std::string name;
	};
}