#pragma once
#include <Wiwa/ecs/systems/System.h>
namespace Wiwa
{
class RewardChest : public System
	{
	public:
		//Called the first frame
		virtual void OnAwake() override;
		//Called after OnAwake()
		virtual void OnInit() override;
		//Called every frame
		virtual void OnUpdate() override;

		virtual void OnCollisionEnter(Object* obj1, Object* obj2) override;

		virtual void OnCollisionExit(Object* obj1, Object* obj2) override;


	private:
		EntityId m_Face1;
		EntityId m_Face2;
		EntityId m_Face3;
		EntityId m_ColliderPreActive;
		std::vector<EntityId> m_EntityChildren;
		EntityManager::ComponentIterator m_Face1It;
		EntityManager::ComponentIterator m_Face2It;
		EntityManager::ComponentIterator m_Face3It;

		float m_Face1Start;
		float m_Face2Start;
		float m_Face3Start;

		float m_Velocity;
		float m_FinalPos;

		bool m_ActiveAnim;

		bool m_Interacted;
	};
}
REGISTER_SYSTEM(Wiwa::RewardChest)
