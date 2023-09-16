#include <wipch.h>
#include "AI_RecastManager.h"
#include <Wiwa/scene/SceneManager.h>
#include <glew.h>
#include "Wiwa/ecs/components/ai/NavMesh.h"
#include <Wiwa/ecs/systems/ai/NavMeshLoadingSystem.h>
#include <Wiwa/AI/AI_Crowd.h>

namespace Wiwa
{
	EntityId RecastManager::m_Id = -1;
	EntityManager::ComponentIterator RecastManager::m_MeshIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };
	RecastCommon* RecastManager::m_RecastMesh = nullptr;
	InputGeom* RecastManager::m_Geom = nullptr;
	BuildContext RecastManager::m_Ctx = BuildContext();
	bool RecastManager::ExecutePipeline()
	{
		return false;
	}

	bool RecastManager::CreateObj()
	{
		if (m_MeshIt.c_id == WI_INVALID_INDEX || m_MeshIt.c_index == WI_INVALID_INDEX)
			return false;

		Wiwa::EntityManager& em = Wiwa::SceneManager::getActiveScene()->GetEntityManager();

		Wiwa::Mesh* mesh = (Wiwa::Mesh*)em.GetComponentByIterator(m_MeshIt);
		if (mesh == nullptr)
		return false;

		Wiwa::Model* mod = Wiwa::Resources::GetResourceById<Wiwa::Model>(mesh->meshId);
		mod->SaveModelAsOBJ(mod, mod->getModelPath());
		return true;
	}

	bool RecastManager::CreateRecast()
	{
		if (m_MeshIt.c_id == WI_INVALID_INDEX || m_MeshIt.c_index == WI_INVALID_INDEX)
			return false;

		Wiwa::EntityManager& em = Wiwa::SceneManager::getActiveScene()->GetEntityManager();
		Wiwa::Mesh* mesh = (Wiwa::Mesh*)em.GetComponentByIterator(m_MeshIt);
		if (mesh == nullptr)
			return false;

		if (m_Geom)
		{
			delete m_Geom;
			m_Geom = nullptr;
		}

		Wiwa::Model* mod = Wiwa::Resources::GetResourceById<Wiwa::Model>(mesh->meshId);
		std::string path = " ";

		if (mod)
			path = mod->getModelPath();

		path = path.substr(0, path.find_last_of('.')) + ".obj";

		// Convert from assets to library path, to navmesh folder, change extension to obj
		std::string library_output = "library\\navmesh\\";
		std::filesystem::path pathObj(path);
		library_output += pathObj.filename().string();

		m_RecastMesh = new RecastSoloMesh();
		m_Geom = new InputGeom();
		
		if (!m_Geom->load(&m_Ctx, library_output))
		{
			delete m_Geom;
			m_Geom = 0;

			// Destroy the sample if it already had geometry loaded, as we've just deleted it!
			if (m_RecastMesh && m_RecastMesh->getInputGeom())
			{
				delete m_RecastMesh;
				m_RecastMesh = 0;
			}
			//m_Ctx.dumpLog("Geom load log %s:", meshName.c_str());
		}

		if (m_RecastMesh)
			m_RecastMesh->setContext(&m_Ctx);

		if (m_RecastMesh && m_Geom)
		{
			m_RecastMesh->handleMeshChanged(m_Geom);
		}

		return true;
	}

	bool RecastManager::Render()
	{

		Wiwa::Camera* camera = Wiwa::SceneManager::getActiveScene()->GetCameraManager().editorCamera;

		GL(Viewport(0, 0, camera->frameBuffer->getWidth(), camera->frameBuffer->getHeight()));
		camera->frameBuffer->Bind(false);

		GL(MatrixMode(GL_PROJECTION));
		GL(LoadMatrixf(glm::value_ptr(camera->getProjection())));
		GL(MatrixMode(GL_MODELVIEW));
		GL(LoadMatrixf(glm::value_ptr(camera->getView())));
		GL(Enable(GL_CULL_FACE));

		if (m_RecastMesh)
			m_RecastMesh->handleUpdate(1.0f/60.0f);

		if (m_RecastMesh)
			m_RecastMesh->handleRender();

		GL(End());
		camera->frameBuffer->Unbind();
		GL(Disable(GL_CULL_FACE));
		return true;
	}

	bool RecastManager::Build()
	{
		if (m_RecastMesh && m_Geom)
		{
			m_RecastMesh->handleBuild();
		}
		return true;
	}

	bool RecastManager::DebugSettings()
	{
		if (m_RecastMesh)
			m_RecastMesh->handleDebugMode();
		return true;
	}

	bool RecastManager::Save()
	{
		if (!m_RecastMesh)
		{
			WI_INFO("Coulnd't save navmesh, as there is no entity navmesh to save");
			return false;
		}

		Wiwa::EntityManager& em = Wiwa::SceneManager::getActiveScene()->GetEntityManager();
		// After all null checks save the file
		std::string path = "assets\\navmesh\\";
		path += Wiwa::SceneManager::getActiveScene()->getName();
		path += ".winavmesh";

		m_RecastMesh->Save(path.c_str());
		path = Wiwa::Resources::_assetToLibPath(path);
		m_RecastMesh->Save(path.c_str());
	
		//// Create an entity and store the path to the file
		//EntityId id = WI_INVALID_INDEX;
		//id = em.GetEntityByName("NavMesh_Data");

		//if (id != WI_INVALID_INDEX)
		//	em.DestroyEntity(id);

		//id = em.CreateEntity("NavMesh_Data");
		//Wiwa::NavMesh navMesh;
		//strcpy(navMesh.filePath, path.c_str());
		//em.AddComponent<Wiwa::NavMesh>(id, navMesh);
		//em.ApplySystem<Wiwa::NavMeshLoadingSystem>(id);
		
		return true;
	}

	bool RecastManager::Load(Scene* scene)
	{
		Wiwa::EntityManager& em = scene->GetEntityManager();
		//EntityId id = em.GetEntityByName("NavMesh_Data");
		
	/*	if (id == WI_INVALID_INDEX)
		{
			WI_INFO("Coulnd't load navmesh, as there is no entity NavMesh_Data");
			return false;
		}*/

	/*	Wiwa::NavMesh* navMesh = em.GetComponent<Wiwa::NavMesh>(id);
		if (!navMesh)
		{
			WI_INFO("Coulnd't load navmesh, as there is no component navMesh");
			return false;
		}*/
		std::string path = "assets\\navmesh\\";
		path += Wiwa::SceneManager::getActiveScene()->getName();
		path += ".winavmesh";


		if (!m_RecastMesh)
		{
			m_RecastMesh = new RecastSoloMesh();
		}
		else
		{
			delete m_RecastMesh;
			m_RecastMesh = nullptr;
			m_RecastMesh = new RecastSoloMesh();
		}

		//m_RecastMesh->Save(path.c_str());

		//std::string path = navMesh->filePath;
		path = Wiwa::Resources::_assetToLibPath(path);
		std::string gsetPath = path;
		gsetPath = gsetPath.substr(0, gsetPath.find_last_of('.')) + ".gset";

		/*std::filesystem::path p = gsetPath;
		if (p.extension() == ".gset") {
			std::filesystem::path rpath = Wiwa::Resources::_assetToLibPath(p.string().c_str());
			std::filesystem::path rp = rpath.remove_filename();
			std::filesystem::create_directories(rp);
			Wiwa::FileSystem::Copy(p.string().c_str(), rpath.string().c_str());
		}*/

		if (m_Geom)
		{
			delete m_Geom;
			m_Geom = nullptr;
		}
		m_Geom = new InputGeom();

		if (!m_Geom->load(&m_Ctx, gsetPath))
		{
			delete m_Geom;
			m_Geom = 0;

			// Destroy the sample if it already had geometry loaded, as we've just deleted it!
			if (m_RecastMesh && m_RecastMesh->getInputGeom())
			{
				delete m_RecastMesh;
				m_RecastMesh = 0;
			}
			//m_Ctx.dumpLog("Geom load log %s:", meshName.c_str());
		}

		if (m_RecastMesh)
			m_RecastMesh->setContext(&m_Ctx);

		if (m_RecastMesh && m_Geom)
		{
			m_RecastMesh->handleMeshChanged(m_Geom);
		}

		if (!m_RecastMesh->Load(path.c_str()))
		{
			WI_INFO("AI PANEL: Couldn't load the navmesh at path {}", path.c_str());
		}

		return true;
	}

	void RecastManager::CalcGridSize(const float* minBounds, const float* maxBounds, float cellSize, int* sizeX, int* sizeZ)
	{
		rcCalcGridSize(minBounds, maxBounds, cellSize, sizeX, sizeZ);
	}

	dtNavMesh* RecastManager::getNavMesh()
	{
		if (m_RecastMesh)
		{
			return m_RecastMesh->getNavMesh();
		}
		return nullptr;
	}

	void RecastManager::Cleanup()
	{
		if (m_RecastMesh)
		{
			delete m_RecastMesh;
			m_RecastMesh = nullptr;
		}

		if (m_Geom)
		{
			delete m_Geom;
			m_Geom = nullptr;
		}

		std::string path = "assets/navmesh/";
		path += Wiwa::SceneManager::getActiveScene()->getName();
		path += ".winavmesh";

		std::string gsetPath = path;
		gsetPath = gsetPath.substr(0, gsetPath.find_last_of('.')) + ".gset";

		std::filesystem::path p = path;

		FileSystem::Remove(path.c_str());
		FileSystem::RemoveAll(gsetPath.c_str());

		path = Wiwa::Resources::_assetToLibPath(path);
		gsetPath = Wiwa::Resources::_assetToLibPath(gsetPath);
		FileSystem::RemoveAll(path.c_str());
		FileSystem::RemoveAll(gsetPath.c_str());
	}

	void RecastManager::DeAllocate()
	{
		if (m_RecastMesh)
		{
			delete m_RecastMesh;
			m_RecastMesh = nullptr;
		}

		if (m_Geom)
		{
			delete m_Geom;
			m_Geom = nullptr;
		}
	}

}


