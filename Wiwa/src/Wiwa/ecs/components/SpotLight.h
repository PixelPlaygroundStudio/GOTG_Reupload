#pragma once
#pragma warning(disable : 4302)
#include <Wiwa/core/Core.h>
#include <glm/glm.hpp>
#include <Wiwa/utilities/Reflection.h>


namespace Wiwa {
	struct WI_API SpotLight {
		glm::vec3 Color;
		float AmbientIntensity;
		float DiffuseIntensity;
		float Constant;
		float Linear;
		float Exp;
		glm::vec3 Direction;
		float Cutoff;
	};
}

WI_REFLECTION_BEGIN(Wiwa::SpotLight)
	WI_REFLECT_MEMBER(Color)
	WI_REFLECT_MEMBER(AmbientIntensity)
	WI_REFLECT_MEMBER(DiffuseIntensity)
	WI_REFLECT_MEMBER(Constant)
	WI_REFLECT_MEMBER(Linear)
	WI_REFLECT_MEMBER(Exp)
	WI_REFLECT_MEMBER(Direction)
	WI_REFLECT_MEMBER(Cutoff)
WI_REFLECTION_END;