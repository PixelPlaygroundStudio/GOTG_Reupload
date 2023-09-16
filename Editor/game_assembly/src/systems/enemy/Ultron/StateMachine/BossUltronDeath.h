#pragma once
#include "BossUltronBase.h"

namespace Wiwa {
	class BossUltronDeathState : public BossUltronBaseState {

		enum class DeathState {
			DEATH_INIT,
			DEATH_INIT_EXPLOSION_1,
			DEATH_INIT_EXPLOSION_2,
			DEATH_INIT_EXPLOSION_3,
			DEATH_EXPLOSIONS,
			DEATH_TALK,
			DEATH_FINISH
		};

	public:
		BossUltronDeathState();
		~BossUltronDeathState();

		void EnterState(BossUltron* enemy) override;
		void UpdateState(BossUltron* enemy) override;
		void ExitState(BossUltron* enemy) override;
		void OnCollisionEnter(BossUltron* enemy, const Object* body1, const Object* body2) override;

	private:

		EntityId SpawnRandomExplosion(BossUltron* enemy);

		glm::vec3 GetExplosionPosition(const glm::vec3& center, float distance, float maxDistance);

		DeathState m_DeathState;

		const char* m_DeathExplosionPath1;
		const char* m_DeathExplosionPath2;
		const char* m_DeathExplosionPath3;
		const char* m_DeathExplosionPath4;

		EntityId m_FinalDeathExplosionId;
	
		float m_TimerDeathExplosions = 0.0f;
		float m_DeathTimer = 0.0f;
	};
}
