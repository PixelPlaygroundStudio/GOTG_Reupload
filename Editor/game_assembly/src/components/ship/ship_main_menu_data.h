#pragma once
#include <Wiwa/utilities/Reflection.h>

namespace Wiwa
{
	struct ShipMainMenuData
	{
		float VelocityMove;
		float VelocityRot;
		float VelocityBackground;
		float PanVelocity;
		bool PanToCamera;
		float PanRotationVelocity;
		bool PanToHala;
	};
}

REFLECTION_BEGIN(Wiwa::ShipMainMenuData)
	REFLECT_MEMBER(VelocityMove)
	REFLECT_MEMBER(VelocityRot)
	REFLECT_MEMBER(VelocityBackground)
	REFLECT_MEMBER(PanVelocity)
	REFLECT_MEMBER(PanToCamera)
	REFLECT_MEMBER(PanRotationVelocity)
	REFLECT_MEMBER(PanToHala)
REFLECTION_END;
