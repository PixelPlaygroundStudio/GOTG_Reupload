#pragma once

#include <Wiwa/core/Core.h>

namespace Wiwa {
	class Camera;
	class Scene;
	class WI_API LightManager
	{
	public:

		LightManager();
		~LightManager();
		

		void AddPointLight(size_t light);
		void RemovePointLight(size_t light);

		void SetScene(Scene* scene);

		void Update(Camera* camera);

		void AddSpotLight(size_t light);
		void RemoveSpotLight(size_t light);

		inline size_t GetPointLightsSize() { return m_PointLights.size(); }
		inline std::vector<size_t>& GetPointLights() { return m_PointLights; }

		inline size_t GetSpotLightsSize() { return m_SpotLights.size(); }
		inline std::vector<size_t>& GetSpotLights() { return m_SpotLights; }


		// Set to -1 if there's no directional lights
		inline void SetDirectionalLight(size_t light) { m_DirectionalLight = light; }
		inline size_t GetDirectionalLight() { return m_DirectionalLight; }
		inline const glm::mat4& GetDirectionalView() { return m_LightView; }
		inline const glm::mat4& GetDirectionalProj() { return m_LightProj; }
		inline const glm::mat4& GetDirectionalMVP() { return m_LightMVP; }
		inline const glm::vec3& GetDirectionalPos() { return m_LightPos; }
	private:
		Scene* m_Scene;

		std::vector<size_t> m_PointLights;
		std::vector<size_t> m_SpotLights;
		size_t m_DirectionalLight = -1;
		glm::mat4 m_LightView;
		glm::mat4 m_LightProj;
		glm::mat4 m_LightMVP;
		glm::vec3 m_LightPos;
	};
}