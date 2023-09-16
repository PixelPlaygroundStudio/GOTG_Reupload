#pragma once

#include <Wiwa/ecs/systems/System.h>
#include <Wiwa/utilities/Reflection.h>

#include <Wiwa/ecs/EntityManager.h>

class TargetSystem : public Wiwa::System {
private:
	Wiwa::EntityManager::ComponentIterator m_ChildTransformIt;

	bool m_Hit;
	float m_HitTimer;
	float m_HitTime;
	float m_RotationSpeed;
public:
	TargetSystem();
	~TargetSystem();

	void OnInit() override;

	void OnUpdate() override;

	void OnCollisionEnter(Wiwa::Object* body1, Wiwa::Object* body2) override;
};

REGISTER_SYSTEM(TargetSystem);