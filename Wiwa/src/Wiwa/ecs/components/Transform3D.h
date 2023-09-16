#pragma once
#pragma warning(disable : 4311)
#pragma warning(disable : 4302)
#include <Wiwa/core/Core.h>
#include <glm/glm.hpp>
#include <Wiwa/utilities/Reflection.h>

#include <glm/mat4x4.hpp>

namespace Wiwa {
	struct WI_API Transform3D {
		// Read-only world position
		glm::vec3 position;
		// Read-only world rotation
		glm::vec3 rotation;
		// Read-only world scale
		glm::vec3 scale;

		// Local position
		glm::vec3 localPosition;
		// Local rotation
		glm::vec3 localRotation;
		// Local scale
		glm::vec3 localScale;

		// World matrix
		glm::mat4 worldMatrix;

		// Local matrix
		glm::mat4 localMatrix;
	};
}

WI_REFLECTION_BEGIN(Wiwa::Transform3D)
	WI_REFLECT_MEMBER(position)
	WI_REFLECT_MEMBER(rotation)
	WI_REFLECT_MEMBER(scale)
	WI_REFLECT_MEMBER(localPosition)
	WI_REFLECT_MEMBER(localRotation)
	WI_REFLECT_MEMBER(localScale)
	WI_REFLECT_MEMBER(worldMatrix)
	WI_REFLECT_MEMBER(localMatrix)
WI_REFLECTION_END;