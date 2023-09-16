#pragma once
#include "BossUltronBase.h"

#define NUMBER_WAVES 3
#define NUMBER_OF_THUNDERS 14
#define ALTITUDE_THUNDERSTORM 30.0f
#define THUNDERS_SPAWN_OFFSET 8.0f

namespace Wiwa {
	class BossUltronProjectileRainAttackState : public BossUltronBaseState {
		enum class ProjectileRainState {
			//MOVE_CENTER,
			PREPARE_RAIN,
			RAIN_ATTACK,
			END_STATE,
		};
	public:
		BossUltronProjectileRainAttackState();
		~BossUltronProjectileRainAttackState();

		void EnterState(BossUltron* enemy) override;
		void UpdateState(BossUltron* enemy) override;
		void ExitState(BossUltron* enemy) override;
		//void OnCollisionEnter(BossUltron* enemy, const Object* body1, const Object* body2) override;
		//void FillPremadePositionAfterRain(BossUltron* enemy, std::vector<glm::vec3>& vec);
		//glm::vec3 GetNewPositionAfterLaser();

	private:

		/*bool SpawnProjectileRain(BossUltron* enemy, const glm::vec3& bull_dir);*/

		glm::vec3 GetRandomPositionInRange(const glm::vec3& position, float range);

		void SpawnThunderStorm(BossUltron* enemy, glm::vec3 thunderPosition, const glm::vec3& bull_dir);

		/*void SetupThunderMark(BossUltron* enemy, int id, glm::vec3 pos, const char* path);*/

		glm::vec3 RandomPoint();

		ProjectileRainState m_RainState;
		std::vector<glm::vec3> m_AfterRainPosition;

		float m_TimerRain;
		float m_TimerThundersSpawn = 0.0f;
		float m_TimerThundersMark = 0.0f;
		float m_TimerThundersMarkDelete = 0.0f;

		bool m_RandomPositionSetted = false;
		int m_RainProjectileCounter;
		const char* m_ThunderMarkPath;

		std::vector<EntityId> m_ThunderMarkIds;
		std::vector<glm::vec3> m_ThunderPositions;
		std::vector<glm::vec3> m_PreviousPoints;
	};
}
