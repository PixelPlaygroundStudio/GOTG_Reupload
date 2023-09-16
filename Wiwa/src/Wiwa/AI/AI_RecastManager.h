#pragma once
#include <Wiwa/ecs/EntityManager.h>
#include <Wiwa/ecs/components/Mesh.h>
#include "RecastObjects/AI_RecastSoloMesh.h"
#include "Geometry/InputGeom.h"

namespace Wiwa
{
	class WI_API RecastManager
	{
	public:
		static EntityManager::ComponentIterator m_MeshIt;
		static EntityId m_Id;
		static RecastCommon* m_RecastMesh;
		static InputGeom* m_Geom;
		static BuildContext m_Ctx;

		static bool ExecutePipeline();
		static bool Render();

		static bool DebugSettings();
		static bool CreateObj();
		static bool CreateRecast();
		static bool Build();

		static bool Save();
		static bool Load(Scene* scene);

		static void Cleanup();
		static void DeAllocate();

		static void CalcGridSize(const float* minBounds, const float* maxBounds, float cellSize, int* sizeX, int* sizeZ);

		static dtNavMesh* getNavMesh();
	};
}
