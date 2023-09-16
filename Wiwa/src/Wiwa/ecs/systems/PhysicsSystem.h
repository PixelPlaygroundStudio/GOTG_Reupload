#pragma once
#include "System.h"
#include "../components/Transform3D.h"
#include "../components/CollisionBody.h"

#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class WI_API PhysicsSystem : public System {
	private:

	public:
		PhysicsSystem();
		~PhysicsSystem();

		void OnAwake() override;

		void OnInit() override;

		void OnUpdate() override;

		void OnDestroy() override;

		void OnSystemAdded() override;

		void OnSystemRemoved() override;

		void OnCollisionEnter(Object* body1, Object* body2) override;
		void OnCollision(Object* body1, Object* body2) override;
		void OnCollisionExit(Object* body1, Object* body2) override;

		void ForceSetPosition(const glm::vec3& new_position);

		void DeleteBody();
		void CreateBody();
		Object* getBody();

		void ActivateBody();
		void DeActivateBody();

		bool OnDisabledFromPool() override;
		void SetBoundingBoxes(Model* root, ColliderCube* cube, Transform3D* transform);
	private:
		Object* obj = nullptr;
	};
}

WI_REGISTER_SYSTEM(Wiwa::PhysicsSystem);