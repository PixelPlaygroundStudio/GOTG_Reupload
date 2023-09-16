#pragma once
#include <Wiwa/ecs/systems/System.h>

#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class  PhylasQuantumSwordSystem : public System {
	private:
		float m_Timer = 0;
		EntityManager::ComponentIterator m_PhylasSwordIt;
		EntityManager::ComponentIterator m_PhylasSwordTransformIt;
		EntityManager::ComponentIterator m_PlayerTransformIt;
		EntityManager::ComponentIterator m_ChildTransformIt;
	public:
		PhylasQuantumSwordSystem();
		~PhylasQuantumSwordSystem();

		void OnAwake() override;

		void OnInit() override;

		void OnUpdate() override;

		void OnDestroy() override;

		void OnCollisionEnter(Object* body1, Object* body2) override;
	};
}

REGISTER_SYSTEM(Wiwa::PhylasQuantumSwordSystem);