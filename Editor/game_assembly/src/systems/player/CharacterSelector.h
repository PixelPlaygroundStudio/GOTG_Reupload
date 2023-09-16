#pragma once
#include <Wiwa/ecs/systems/System.h>
namespace Wiwa
{
class CharacterSelector : public System
	{
	public:
		//Called the first frame
		virtual void OnAwake() override;
		//Called after OnAwake()
		virtual void OnInit() override;
		//Called every frame
		virtual void OnUpdate() override;

		virtual void OnCollisionEnter(Object* obj1, Object* obj2) override;
		virtual void OnCollision(Object* obj1, Object* obj2) override;
		virtual void OnCollisionExit(Object* obj1, Object* obj2) override;
	private:
		bool m_Activated;
		bool m_CanInteract;

		const char* starlord_path = "library/characters/starlord/sk_starlord01.wimodel";
		const char* rocket_path = "library/characters/rocket/sk_rocket01.wimodel";
	};
}
REGISTER_SYSTEM(Wiwa::CharacterSelector)
