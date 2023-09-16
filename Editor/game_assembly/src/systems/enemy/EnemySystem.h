#pragma once
#include <Wiwa/ecs/systems/System.h>
#include <Wiwa/utilities/Reflection.h>
//#include <Wiwa/ecs/systems/AudioSystem.h>
//#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>
//#include <Wiwa/ecs/systems/OzzAnimationSystem.h>

namespace Wiwa {
	class  EnemySystem : public System {
	public:
		EntityManager::ComponentIterator m_EnemyStateIt;
		EntityManager::ComponentIterator m_StatsIt;
		EntityManager::ComponentIterator m_Health;
		EntityManager::ComponentIterator m_ColliderIt;
		EntityManager::ComponentIterator m_AgentIt;
		EntityManager::ComponentIterator m_NavAgentIt;
		EntityManager::ComponentIterator m_TransformIt;
		EntityManager::ComponentIterator m_AudioSourceIt;

		EntityManager::ComponentIterator m_PlayerTransformIt;
		EntityManager::ComponentIterator m_PlayerStatsIt;
		SystemHash m_PhysicsSystemHash;

		class NavAgentSystem* m_NavAgentSys = nullptr;
		class OzzAnimationSystem* m_AnimatorSys = nullptr;
		class AudioSystem* m_AudioSys = nullptr;

		EntityId m_PlayerId;
		EntityId m_WaveId = -1;
		bool m_WasSpawnedBySpawner = false;

		float m_TimerSlow = 0.0f;
		float m_SlowPercentage = 0.0f;
		float m_PreviousSpeed = 0.0f;
	public:
		EnemySystem();
		~EnemySystem();

		void OnAwake() override;

		void OnInit() override;

		void OnUpdate() override;

		void OnDestroy() override;

		void OnCollisionEnter(Object* body1, Object* body2) override;

		virtual void ReceiveDamage(int damage);

		bool OnEnabledFromPool() override;

		bool OnDisabledFromPool() override;

		void LookAt(const glm::vec3& target_look, float rotation_speed);

		void LookAtWithoutInterpolation(const glm::vec3& target_look);

		void RotateTo(const glm::vec3& target);

		void SetStatsFromTable();

		void SlowForTime(float time, float percentage);

		void DisplayStats(glm::vec3 position,std::string text, glm::vec4 color);
		void DisplayGold(glm::vec3 position,std::string text, glm::vec4 color);
	};
}

REGISTER_SYSTEM(Wiwa::EnemySystem);