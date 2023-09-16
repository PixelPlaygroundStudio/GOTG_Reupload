#pragma once
#pragma warning(disable : 4302)
#include <Wiwa/core/Core.h>
#include <glm/glm.hpp>
#include <Wiwa/utilities/Reflection.h>


namespace Wiwa {
	struct WI_API PointLight {
		glm::vec3 Color;
		float AmbientIntensity;
		float DiffuseIntensity;
		float Constant;
		float Linear;
		float Exp;
	};
}

WI_REFLECTION_BEGIN(Wiwa::PointLight)
	WI_REFLECT_MEMBER(Color)
	WI_REFLECT_MEMBER(AmbientIntensity)
	WI_REFLECT_MEMBER(DiffuseIntensity)
	WI_REFLECT_MEMBER(Constant)
	WI_REFLECT_MEMBER(Linear)
	WI_REFLECT_MEMBER(Exp)
	WI_REFLECTION_END;