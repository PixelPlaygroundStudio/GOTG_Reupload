#pragma once
#include <wipch.h>
#include "System.h"
#include <Wiwa/core/Core.h>

#include "Wiwa/ecs/components/Transform3D.h"
#include "Wiwa/ecs/components/ParticleEmitterComponent.h"
#include <Wiwa/utilities/render/shaders/Shader.h>
#include <Wiwa/utilities/render/Uniforms.h>
#include <Wiwa/core/Resources.h>

#include <Wiwa/utilities/Reflection.h>

#include <glm/glm.hpp>

namespace Wiwa
{
	struct Particle
	{
		Particle() :
			life_time(0.0f),
			life_time_start(0.0f),
			life_percentage(0.0f),
			//position(0.0f),
			startPosition(0.0f),
			startRotation(0.0f),
			angularVelocity(0.0f),
			//scale(0.0f),
			growthVelocity(0.0f),
			velocity(0.0f),
			color(0.0f),
			timeAlive(0.0f),
			uniformGrowthVal(0.f) {};

		Particle(float _life, glm::vec3 _startPosition, glm::vec3 _rotation, glm::vec3 _angularVelocity, glm::vec3 _scale, glm::vec3 _growthVelocity, glm::vec3 _velocity, glm::vec4 _color)
		{
			transform.localPosition= _startPosition;
			transform.localRotation = _rotation;
			transform.localScale = _scale;
			life_time = _life;
			life_time_start = _life;
			life_percentage = 0.0f;
			startPosition = _startPosition;
			startRotation = _rotation;
			//position = startPosition;
			//rotation = _rotation;
			angularVelocity = _angularVelocity;
			//scale = _scale;
			growthVelocity = _growthVelocity;
			velocity = _velocity;
			color = _color;	
			timeAlive = 0.0f;
			uniformGrowthVal = 0;

		};
		
		float life_time = 0;
		float life_time_start = 0;
		float life_percentage = 0;
		float timeAlive = 0.f;
		float uniformGrowthVal = 0;
		Transform3D transform;
		//glm::vec3 position;
		glm::vec3 startPosition;
		glm::vec3 velocity;
		glm::vec3 startRotation;
		glm::vec3 angularVelocity;
		//glm::vec3 scale;
		glm::vec3 growthVelocity;
		glm::vec4 color;
		//glm::mat4 transform;
	};

	class WI_API ParticleSystemHolder : public System
	{
	public:
		ParticleSystemHolder() {}
		~ParticleSystemHolder() {}

		void OnUpdate() override;

	};

	class WI_API ParticleSystem : public System
	{
	public: 
		ParticleSystem();
		~ParticleSystem();

		void OnSystemAdded() override;

		void OnAwake() override;

		void OnInit() override;

		bool OnEnabledFromPool() override;

		void OnUpdate() override;

		void OnDestroy() override;

		bool OnDisabledFromPool() override;

		void Render(Particle& particle);

		void SetValues(ParticleEmitterComponent settings);

		void SpawnParticle(Particle& particle);
		void SpawnParticleSet();

		void UpdateParticleLife(Particle& particle, float deltaTime);
		void SetParticleLifeTime(Particle& particle, float lifeTime);

		unsigned int FirstUnusedParticle();

		unsigned int GetVAO() { return m_VAO; }

		unsigned int  m_MaxParticles = 10;

		bool m_Loop;

		float m_Duration = 0.0f;

		glm::vec4 InterpolateVec4(glm::vec4 valStart, glm::vec4 valEnd, float currentPercentage, float targetPercentage);
		float InterpolateFloat(float valStart, float valEnd, float currentPercentage, float targetPercentage);
		glm::vec3 InterpolateVec3(glm::vec3 valStart, glm::vec3 valEnd, float currentPercentage, float targetPercentage);

		//Emits a batch of particles based on the amount given.
		void EmitParticleBatch(int amount);

		//Emits a batch of particles based on the Emitter's spawn amount settings.
		void EmitParticleBatch();

		//Sets the Active state of the Emitter.
		void SetActive(bool active, bool forceDeactivateParticles = false);

		//Sets the material of the Particles the Emitter generates;
		void SetMaterial(const char* materialPath);

		//Sets the mesh of the Particles the Emitter generates;
		void SetMesh(const char* meshPath);

		//Sets the timer of the Particles the Emitter generates;
		void SetTimer(float time);

		//Deactivates all active particles;
		void DeactivateParticles();

		//Restart the emitter;
		void RestartEmitter(float timer = 0.f);

		//Set color of a certain node. Uses the current color system of the emitter;
		void SetColor(int index, glm::vec4 color);

		//returns the material the particle system is using
		Material* GetMaterial() { return m_Material; }

	private:

		void SetEmitterBools(ParticleEmitterComponent* emitter);
		void FixBool(bool& _bool, bool value = false);

		unsigned int m_VAO = -1;

		unsigned int m_LastUsedParticle = 0;
		
		unsigned int m_AvailableParticles = 0;

		float m_SpawnTimer;

		bool m_AlreadySpawned;

		std::vector<Particle> m_Particles;

		Material* m_Material = nullptr;

		Model* m_Model = nullptr;
	};
}
REGISTER_SYSTEM(Wiwa::ParticleSystem);
REGISTER_SYSTEM(Wiwa::ParticleSystemHolder);

