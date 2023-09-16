#pragma once
#include "../EnemySystem.h"

namespace Wiwa {
	class  EnemySubjugator : public EnemySystem {
	public:
		class SubjugatorBaseState* m_CurrentState;
		class SubjugatorBaseState* m_SpawnState;
		class SubjugatorBaseState* m_IdleState;
		class SubjugatorBaseState* m_ChasingState;
		class SubjugatorBaseState* m_AttackingState;
		class SubjugatorBaseState* m_DeathState;
		class SubjugatorBaseState* m_HitState;

		EntityManager::ComponentIterator m_Subjugator;
		EntityManager::ComponentIterator m_Hand1It;
		EntityManager::ComponentIterator m_Hand2It;
		EntityManager::ComponentIterator m_Hand3It;
		EntityManager::ComponentIterator m_Hand4It;

		float m_Timer;
		//float m_RangeOfAttack = 0.0f;
		int m_MinimumPath = 0;
	public:
		EnemySubjugator();
		~EnemySubjugator();

		void OnAwake() override;

		void OnInit() override;

		void OnUpdate() override;

		void OnDestroy() override;

		void OnCollisionEnter(Object* body1, Object* body2) override;

		void ReceiveDamage(int damage) override;

		void SwitchState(SubjugatorBaseState* state);

		bool OnEnabledFromPool() override;

		bool OnDisabledFromPool() override;
	};
}

REGISTER_SYSTEM(Wiwa::EnemySubjugator);