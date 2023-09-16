#pragma once
#pragma warning(disable : 4302)
#include <Wiwa/core/Core.h>
#include <glm/glm.hpp>
#include <Wiwa/utilities/Reflection.h>


namespace Wiwa {
	struct WI_API DirectionalLight {
		glm::vec3 Color;
		float AmbientIntensity;
		float DiffuseIntensity;
	};
}

WI_REFLECTION_BEGIN(Wiwa::DirectionalLight)
	WI_REFLECT_MEMBER(Color)
	WI_REFLECT_MEMBER(AmbientIntensity)
	WI_REFLECT_MEMBER(DiffuseIntensity)
WI_REFLECTION_END;
