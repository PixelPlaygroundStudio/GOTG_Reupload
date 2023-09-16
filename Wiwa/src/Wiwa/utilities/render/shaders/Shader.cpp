#include <wipch.h>

#include "Shader.h"
#include <Wiwa/utilities/render/Material.h>

#include <fstream>
#include <string>
#include <sstream>
#include "Wiwa/core/Resources.h"
#include <glew.h>
#include <rapidjson.h>

namespace Wiwa {
	Shader::Shader()
	{
		m_AllOk = false;
		m_CompileState = State::ToCompile;
	}


	Shader::~Shader()
	{
		GL(DeleteProgram(m_IDprogram));
		m_Uniforms.clear();
	}

	void Shader::CreateDefault(const char* path)
	{
		std::string file = path;
		file += ".vs";

		std::ofstream vertexFile(file.c_str());
		vertexFile << 
			"#version 330 core\n\n// Positions/Coordinates\nlayout (location = 0) in vec3 aPos;\n// Colors\nlayout (location = 1) in vec3 aColor;\n// Texture Coordinates\nlayout (location = 2) in vec2 aTex;\n\n\n// Outputs the color for the Fragment Shader\nout vec3 color;\n// Outputs the texture coordinates to the fragment shader\nout vec2 texCoord;\n\n\n//Necesary to render the model DON'T touch\nuniform mat4 u_Model;\nuniform mat4 u_Proj;\nuniform mat4 u_View;\n\n\n\n\nvoid main()\n{\n\tgl_Position = u_Proj * u_View * u_Model * vec4(aPos, 1.0);\n\n\t// Assigns the colors from the Vertex Data to \"color\"\n\tcolor = aColor;\n\t// Assigns the texture coordinates from the Vertex Data to \"texCoord\"\n\ttexCoord = aTex;\n}";
		vertexFile.close();
		
		file = path;
		file += ".fs";
		std::ofstream fragmentFile(file.c_str());
		fragmentFile <<
			"#version 330 core\n\n// Outputs colors in RGBA\nout vec4 FragColor;\n\n\n// Inputs the color from the Vertex Shader\nin vec3 color;\n// Inputs the texture coordinates from the Vertex Shader\nin vec2 texCoord;\n\n// Gets the Texture Unit from the main function\nuniform sampler2D u_Tex0;\n\n\nvoid main()\n{\n\tFragColor = texture(u_Tex0, texCoord);\n}";
		fragmentFile.close();

	}

	void Shader::Init(const char* filename)
	{
		Compile(filename);
	}

	void Shader::Compile(const char* filename)
	{
		std::string vertexFile = filename;
		vertexFile += ".vs";

		std::string fragmentFile = filename;
		fragmentFile += ".fs";

		std::string geometryFile = filename;
		geometryFile += ".gs";

		std::string* vertexShaderSourceStr = getFileData(vertexFile.c_str());

		if (!vertexShaderSourceStr) {
			std::string msg = "Couldn't open file: ";
			msg += vertexFile;
			WI_CORE_CRITICAL(msg.c_str());
			m_CompileState = State::Error;
			return;
		}

		const char* vertexShaderSource = vertexShaderSourceStr->c_str();

		std::string* fragmentShaderSourceStr = getFileData(fragmentFile.c_str());

		if (!fragmentShaderSourceStr) {
			std::string msg = "Couldn't open file: ";
			msg += fragmentFile;
			WI_CORE_CRITICAL(msg.c_str());
			m_CompileState = State::Error;
			return;
		}

		const char* fragmentShaderSource = fragmentShaderSourceStr->c_str();

		std::string* geometryShaderSourceStr = getFileData(geometryFile.c_str());
		bool hasGS = geometryShaderSourceStr;

		bool retflag;
		
		CompileFiles(vertexShaderSource, fragmentShaderSource, hasGS, geometryShaderSourceStr, retflag);
		delete vertexShaderSourceStr;
		delete fragmentShaderSourceStr;
		delete geometryShaderSourceStr;
		
		if (retflag) return;


		m_AllOk = true;

		m_Path = filename;
	}

	void Shader::CompileWishader(const char* filename)
	{
		std::string file = filename;
		file += ".wishader";
		JSONDocument shaderFile;

		if (!shaderFile.load_file(file.c_str()))
			return;
		std::string vertexShader;
		std::string fragmentShader;
		std::string geometryShader;

		bool hasGeometry = false;

		if (shaderFile.HasMember("vertex"))
			vertexShader = shaderFile["vertex"].get<const char*>();
		if (shaderFile.HasMember("fragment"))
			fragmentShader = shaderFile["fragment"].get<const char*>();
		if (shaderFile.HasMember("geometry"))
			geometryShader = shaderFile["geometry"].get<const char*>();

		if (!geometryShader.empty())
			hasGeometry = true;

		bool ret = false;
		m_Path = filename;
		CompileFiles(vertexShader.c_str(), fragmentShader.c_str(), hasGeometry, &geometryShader, ret);
	}

	void Shader::CompileFiles(const char* vertexShaderSource, const char* fragmentShaderSource, bool hasGS, std::string* geometryShaderSourceStr, bool& retflag)
	{
		retflag = true;
		unsigned int vertexShader = GL(CreateShader(GL_VERTEX_SHADER));
		GL(ShaderSource(vertexShader, 1, &vertexShaderSource, NULL)); 
		GL(CompileShader(vertexShader));

		int success;
		char infoLog[512];

		GL(GetShaderiv(vertexShader, GL_COMPILE_STATUS, &success));

		if (!success) {
			GL(GetShaderInfoLog(vertexShader, 512, NULL, infoLog));

			std::string msg = "Vertex shader compile error: ";
			msg += infoLog;

			WI_CORE_CRITICAL(msg.c_str());
			m_CompileState = State::Error;
			return;
		}

		unsigned int fragmentShader = GL(CreateShader(GL_FRAGMENT_SHADER));
		GL(ShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL));
		GL(CompileShader(fragmentShader));

		GL(GetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success));

		if (!success) {
			GL(GetShaderInfoLog(fragmentShader, 512, NULL, infoLog));
			std::string msg = "Fragment shader compile error: ";
			msg += infoLog;

			WI_CORE_CRITICAL(msg.c_str());
			m_CompileState = State::Error;
			return;
		}

		m_IDprogram = GL(CreateProgram());
		GL(AttachShader(m_IDprogram, vertexShader));
		GL(AttachShader(m_IDprogram, fragmentShader));

		unsigned int geometryShader;
		if (hasGS)
		{
			const char* geometryShaderSource = geometryShaderSourceStr->c_str();
			geometryShader = GL(CreateShader(GL_GEOMETRY_SHADER));
			GL(ShaderSource(geometryShader, 1, &geometryShaderSource, NULL));
			GL(CompileShader(geometryShader));

			GL(GetShaderiv(geometryShader, GL_COMPILE_STATUS, &success));

			if (!success) {
				GL(GetShaderInfoLog(geometryShader, 512, NULL, infoLog));
				std::string msg = "Geometry shader compile error: ";
				msg += infoLog;

				WI_CORE_CRITICAL(msg.c_str());
				m_CompileState = State::Error;
				return;
			}
			GL(AttachShader(m_IDprogram, geometryShader));
		}

		
		GL(LinkProgram(m_IDprogram));
		
		GL(GetProgramiv(m_IDprogram, GL_LINK_STATUS, &success));
		
		if (!success) {
			GL(GetProgramInfoLog(m_IDprogram, 512, NULL, infoLog));
			std::string msg = "Shader program compile error: ";
			msg += infoLog;

			WI_CORE_CRITICAL(msg.c_str());
			m_CompileState = State::Error;
			return;
		}

		
		GL(DeleteShader(vertexShader));
		GL(DeleteShader(fragmentShader));
		if (hasGS) {
			GL(DeleteShader(geometryShader));
		}

		retflag = false;

		m_CompileState = State::Compiled;
		
		m_Model = GL(GetUniformLocation(m_IDprogram, "u_Model"));
		m_Proj = GL(GetUniformLocation(m_IDprogram, "u_Proj"));
		m_View = GL(GetUniformLocation(m_IDprogram, "u_View"));
		m_UCamera = GL(GetUniformLocation(m_IDprogram, "u_CameraPosition"));

		LightLocations.CameraLoc = GL(GetUniformLocation(m_IDprogram, "u_CameraPosition"));
		LightLocations.DirectionalLightBC = GL(GetUniformLocation(m_IDprogram, "u_DirectionalLight.Base.Color"));
		LightLocations.DirectionalLightBaseAmbientIntensity = GL(GetUniformLocation(m_IDprogram, "u_DirectionalLight.Base.AmbientIntensity"));
		LightLocations.DirectionalLightBaseDiffuseIntensity = GL(GetUniformLocation(m_IDprogram, "u_DirectionalLight.Base.DiffuseIntensity"));
		LightLocations.DirectionalLightDirection = GL(GetUniformLocation(m_IDprogram, "u_DirectionalLight.Direction"));
		LightLocations.NumPointLights = GL(GetUniformLocation(m_IDprogram, "u_NumPointLights"));

		for (size_t i = 0; i < 32; i++)
		{
			std::string num = std::to_string(i);
			LightLocations.PointLights[i].BaseColor = GL(GetUniformLocation(m_IDprogram, ("u_PointLights[" + num + "].Base.Color").c_str()));
			LightLocations.PointLights[i].BaseAmbientIntensity = GL(GetUniformLocation(m_IDprogram, ("u_PointLights[" + num + "].Base.AmbientIntensity").c_str()));
			LightLocations.PointLights[i].BaseDiffuseIntensity = GL(GetUniformLocation(m_IDprogram, ("u_PointLights[" + num + "].Base.DiffuseIntensity").c_str()));
			LightLocations.PointLights[i].LocalPos = GL(GetUniformLocation(m_IDprogram, ("u_PointLights[" + num + "].LocalPos").c_str()));
			LightLocations.PointLights[i].AttenConstant = GL(GetUniformLocation(m_IDprogram, ("u_PointLights[" + num + "].Atten.Constant").c_str()));
			LightLocations.PointLights[i].AttenLinear = GL(GetUniformLocation(m_IDprogram, ("u_PointLights[" + num + "].Atten.Linear").c_str()));
			LightLocations.PointLights[i].AttenExp = GL(GetUniformLocation(m_IDprogram, ("u_PointLights[" + num + "].Atten.Exp").c_str()));
		}

		LightLocations.DirLightMVP = GL(GetUniformLocation(m_IDprogram, "u_LightMVP"));
		LightLocations.DirLightPos = GL(GetUniformLocation(m_IDprogram, "u_LightPos"));
		LightLocations.RecieveShadows = GL(GetUniformLocation(m_IDprogram, "u_RecieveShadows"));
	}

	bool Shader::LoadFromWiasset(const char* filename)
	{
		std::string file_name = filename;
		file_name += ".wishader";
		JSONDocument shaderFile;

		if (!shaderFile.load_file(file_name.c_str()))
		{
			Resources::SetAssetPath(file_name);
			if (!shaderFile.load_file(file_name.c_str()))
				return false;
		}
		
		std::string vertexShader;
		std::string fragmentShader;
		std::string geometryShader;

		bool hasGeometry = false;

		if (shaderFile.HasMember("vertex"))
			vertexShader = shaderFile["vertex"].get<const char*>();
		if (shaderFile.HasMember("fragment"))
			fragmentShader = shaderFile["fragment"].get<const char*>();
		if (shaderFile.HasMember("geometry"))
			geometryShader = shaderFile["geometry"].get<const char*>();
		
		if (!geometryShader.empty())
			hasGeometry = true;

		bool ret = false;
		CompileFiles(vertexShader.c_str(), fragmentShader.c_str(), hasGeometry, &geometryShader, ret);

		if (shaderFile.HasMember("uniforms"))
		{
			JSONValue uniforms = shaderFile["uniforms"];
			rapidjson::Value* val = uniforms.getValue();
			for (rapidjson::Value::MemberIterator p = val->MemberBegin(); p != val->MemberEnd(); ++p)
			{
				addUniform(p->name.GetString(), (UniformType)p->value.GetInt());
			}
		}
		/*std::string file_path = filename;
		Resources::standarizePath(file_path);
		std::filesystem::path import_file = file_path;
		std::filesystem::path export_file = Resources::_assetToLibPath(file_path);
		export_file.replace_extension(".wishader");
		std::filesystem::path export_path = export_file.parent_path();

		if (Resources::_preparePath(export_path.string())) {
			shaderFile.save_file(export_file.string().c_str());
			Resources::CreateMeta<Shader>(file_path.c_str());
		}*/

		return true;
	}

	void Shader::Bind()
	{
		if (m_CompileState != State::Compiled)
			return;
		GL(UseProgram(m_IDprogram));
	}

	void Shader::UnBind()
	{
		GL(UseProgram(0));
	}

	void Shader::Delete()
	{
		GL(DeleteProgram(m_IDprogram));
	}

	std::string* Shader::getFileData(const char* file)
	{
		std::fstream shaderFile;

		shaderFile.open(file, std::ios::in);

		if (!shaderFile.is_open()) return NULL;

		std::stringstream buffer;

		buffer << shaderFile.rdbuf();

		shaderFile.close();

		return new std::string(buffer.str());
	}

	unsigned int Shader::getUniformLocation(const char* uniform_name)
	{
		unsigned int uloc = GL(GetUniformLocation(m_IDprogram, uniform_name));

		return uloc;
	}

	void Shader::setUniformInt(unsigned int uniform_id, int value)
	{
		GL(UseProgram(m_IDprogram));
		GL(Uniform1i(uniform_id, value));
	}

	void Shader::setUniformUInt(unsigned int uniform_id, unsigned int value)
	{
		GL(UseProgram(m_IDprogram));
		GL(Uniform1ui(uniform_id, value));
	}

	void Shader::setUniformMat4(unsigned int uniform_id, glm::mat4 value)
	{
		GL(UseProgram(m_IDprogram));
		GL(UniformMatrix4fv(uniform_id, 1, GL_FALSE, glm::value_ptr(value)));
	}

	void Shader::setUniformFloat(unsigned int uniform_id, float value)
	{
		GL(UseProgram(m_IDprogram));
		GL(Uniform1f(uniform_id, value));
	}

	void Shader::setUniformVec3(unsigned int uniform_id, glm::vec3 value)
	{
		GL(UseProgram(m_IDprogram));
		GL(Uniform3f(uniform_id, value.x, value.y, value.z));
	}

	void Shader::setUniformVec4(unsigned int uniform_id, glm::vec4 value)
	{
		GL(UseProgram(m_IDprogram));
		GL(Uniform4f(uniform_id, value.r, value.g, value.b, value.a));
	}

	void Shader::addUniform(const char* name, const UniformType type)
	{
		UniformField* uniform = getUniform(name);
		//Checking if we are
		if (uniform)
		{
			if (uniform->type == type)
				return;
			
			uniform->type = type;
			uniform->location = GL(GetUniformLocation(m_IDprogram, name));
			return;
		}
		UniformField field;
		field.name = name;
		field.type = type;
		field.location = GL(GetUniformLocation(m_IDprogram, name));


		m_Uniforms.emplace_back(field);
	}

	void Shader::deleteUniform(const char* name)
	{
		for (size_t i = 0; i < m_Uniforms.size(); i++)
		{
			if (m_Uniforms[i].name == name)
			{
				m_Uniforms.erase(m_Uniforms.begin() + i);
				i--;
			}
		}
	}

	void Shader::setUniformType(const char* name, const UniformType type)
	{
		UniformField* uniform = getUniform(name);
		if (!uniform)
		{
			WI_CORE_ERROR("Uniform name can't be find");
			return;
		}
		uniform->type = type;
	}

	void Shader::setUniformName(const char* oldName, const char* newName)
	{
		UniformField* uniform = getUniform(oldName);
		if (!uniform)
		{
			WI_CORE_ERROR("Uniform name can't be find in shader");
			return;
		}
		uniform->name = newName;
	}

	void Shader::SetMVP(const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj)
	{
		GL(UniformMatrix4fv(m_Model, 1, GL_FALSE, glm::value_ptr(model)));
		GL(UniformMatrix4fv(m_View, 1, GL_FALSE, glm::value_ptr(view)));
		GL(UniformMatrix4fv(m_Proj, 1, GL_FALSE, glm::value_ptr(proj)));
	}

	void Shader::SetCameraPos(const glm::vec3& position)
	{
		setUniform(m_UCamera, position);
	}

	Wiwa::UniformField* Shader::getUniform(const char* name)
	{
		if (m_Uniforms.empty())
			return nullptr;
		for (size_t i = 0; i < m_Uniforms.size(); i++)
		{
			if (m_Uniforms[i].name == name)
			{
				return &m_Uniforms[i];
			}
		}
		return nullptr;
	}

	void Shader::Save()
	{
		for (const auto& mat : m_MatRefs)
		{
			mat->Refresh();
		}
		std::string assetPath = m_Path;
		Resources::SetAssetPath(assetPath);
		Resources::Import<Shader>(assetPath.c_str(), this);
	}

}