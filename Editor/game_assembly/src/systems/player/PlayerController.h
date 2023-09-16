#pragma once
#include <Wiwa/ecs/systems/System.h>
#include <Wiwa/utilities/Reflection.h>
#include <Wiwa\ecs\systems\PhysicsSystem.h>
#include <Wiwa/ecs/systems/AudioSystem.h>
#include <Wiwa/audio/Audio.h>
#include <Wiwa/ecs/systems/OzzAnimationSystem.h>

namespace Wiwa {
	struct StarLordShooter;
	struct RocketShooter;
	struct Character;

	class PlayerController : public System {
		bool m_PlayerSet = false;
		EntityId playerId = -1;
	public:
		PlayerController();
		virtual ~PlayerController();

		void OnAwake() override;
		void OnInit() override;
		void OnDestroy() override;

		void OnUpdate() override;

		StarLordShooter* GetStarLord();
		RocketShooter* GetRocket();
		Character* GetCharacter();
		Transform3D* GetTransform();
		CollisionBody* GetRigidBody();
		
		WI_HARD_INL EntityId GetEntity() { return m_EntityId; }
		WI_HARD_INL float GetDirection() { return m_Direction; }
		WI_HARD_INL glm::vec2 GetInput() { return m_MovementInput; }
		WI_HARD_INL glm::vec2 GetShootInput() { return m_ShootInput; }
		WI_HARD_INL EntityManager& GetEntityManager() { return m_Scene->GetEntityManager(); }
		
		void SetVelocity(const glm::vec2& velocity) { m_CurrentVelocity = velocity; }

		bool IsDashEnable() { return m_DashEnable; }
		
		void SetDashEnable(bool value) { m_DashEnable = value; }

		void SetDirection(const float value) { m_Direction = value; }

		OzzAnimationSystem* GetAnimator();
		PhysicsSystem* GetPhysics();
		Transform3D* GetFirePosition(const char* name);
		AudioSystem* GetAudio();

		void SpawnStarLordBullet(Transform3D& transform, const Character& character);
		void SpawnRocketBullet(Transform3D& transform, const Character& character);
		void SpawnStarLordUltimate(Transform3D& transform, const Character& character);
		void SpawnRocketUltimate(Transform3D& transform, const Character& character);


		void SetPlayerRotation(const float angle);
		

	public:
		bool IsDashing;
	private:
		glm::vec2 GetMovementInput();
		glm::vec2 GetShootingInput();
	protected:
		EntityManager::ComponentIterator m_TransformIt;
		EntityManager::ComponentIterator m_StatsIt;
		EntityManager::ComponentIterator m_RigidbodyIt;
		EntityManager::ComponentIterator m_ShooterIt;
		EntityManager::ComponentIterator m_RocketIt;
		
		glm::vec2 m_ShootInput;
		glm::vec2 m_MovementInput;

		glm::vec2 m_CurrentVelocity;

		float m_Direction;
		bool m_DashEnable;
		float m_CooldownTimer;
	};
}

REGISTER_SYSTEM(Wiwa::PlayerController);