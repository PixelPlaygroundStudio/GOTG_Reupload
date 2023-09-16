#pragma once
#pragma warning(disable : 4302)
#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/math/Vector3f.h>
#include <Wiwa/utilities/Reflection.h>
#include <Wiwa/core/Resources.h>

namespace Wiwa {
	struct WI_API Mesh {
		ResourceId meshId;
		ResourceId materialId;
		bool drawChildren;
		size_t modelIndex;
		char mesh_path[128];
		char mat_path[128];
		bool castShadows;
		bool shadowRecieve;
	};
}

WI_REFLECTION_BEGIN(Wiwa::Mesh)
	WI_REFLECT_MEMBER(meshId)
	WI_REFLECT_MEMBER(materialId)
	WI_REFLECT_MEMBER(drawChildren)
	WI_REFLECT_MEMBER(modelIndex)
	WI_REFLECT_MEMBER(mesh_path)
	WI_REFLECT_MEMBER(mat_path)
	WI_REFLECT_MEMBER(castShadows)
	WI_REFLECT_MEMBER(shadowRecieve)
WI_REFLECTION_END;