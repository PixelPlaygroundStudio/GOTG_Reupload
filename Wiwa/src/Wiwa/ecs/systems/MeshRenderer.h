#pragma once
#pragma warning(disable : 4267)
#include "System.h"
#include "../components/Transform3D.h"
#include "../components/Mesh.h"
#include "../components/AudioSource.h"
#include "../components/AnimatorComponent.h"


#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	class WI_API MeshRenderer : public System {
	private:

	public:
		MeshRenderer();
		~MeshRenderer();

		void OnUpdate() override;

		bool OnComponentAdded(byte* data, const Type* type) override { return true; }

		bool OnComponentRemoved(byte* data, const Type* type) override { return true; }

		Material* GetMaterial();
	};
}

WI_REGISTER_SYSTEM(Wiwa::MeshRenderer);