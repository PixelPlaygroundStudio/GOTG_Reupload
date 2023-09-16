#pragma once
#include "System.h"
#include "../components/Transform3D.h"
#include "../components/AudioSource.h"

#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class WI_API RayCastSystem : public System {
	private:

	public:
		RayCastSystem();
		~RayCastSystem();

		void OnUpdate() override;
	};
}

WI_REGISTER_SYSTEM(Wiwa::RayCastSystem);