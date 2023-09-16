#include "Wiwa/ecs/Systems.h"

#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class MainMenuUISystem : public System {
	public:

	public:
		MainMenuUISystem();
		~MainMenuUISystem();

		void OnAwake() override;

		void OnInit() override;

		void OnUpdate() override;

		void OnDestroy() override;
	};
}

REGISTER_SYSTEM(Wiwa::MainMenuUISystem);