#include "wipch.h"

#include "EntityManager.h"
#include "systems/System.h"

#include <Wiwa/core/Application.h>
#include <Wiwa/utilities/render/Model.h>
#include <Wiwa/utilities/render/Material.h>
#include <Wiwa/core/Renderer3D.h>
#include <Wiwa/scene/Scene.h>

#include <Wiwa/ecs/components/Mesh.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Wiwa {
	
	EntityManager::EntityManager()
	{
		m_ComponentIdCount = 0;
		m_InitSystemsOnApply = true;
	}

	EntityManager::~EntityManager()
	{
	}

	void EntityManager::Clear()
	{
		// Destroy entity systems
		size_t entitySize = m_EntitySystems.size();

		for (size_t i = 0; i < entitySize; i++) {
			size_t system_size = m_EntitySystems[i].size();

			for (size_t j = 0; j < system_size; j++) {
				System* s = m_EntitySystems[i][j];

				if (s)
				{
					s->Destroy();
					delete s;
				}
			}
		}

		// Destroy all components at once
		size_t c_size = m_Components.size();

		for (size_t i = 0; i < c_size; i++) {
			delete[] m_Components[i];
		}
	}

	void EntityManager::RemoveEntity(EntityId eid)
	{
		m_EntitiesRemoved.push_back(eid);

		m_EntityActive[eid] = false;

		// Remove entity from alive entities vector
		size_t ealive = m_EntitiesAlive.size();

		for (size_t i = 0; i < ealive; i++) {
			if (m_EntitiesAlive[i] == eid) {
				m_EntitiesAlive.erase(m_EntitiesAlive.begin() + i);
				break;
			}
		}
		
		EntityId p_ent = m_EntityParent[eid];

		// Remove entity from parent alive entities vector
		if (p_ent == eid) {
			size_t pealive = m_ParentEntitiesAlive.size();

			for (size_t i = 0; i < pealive; i++) {
				if (m_ParentEntitiesAlive[i] == eid) {
					m_ParentEntitiesAlive.erase(m_ParentEntitiesAlive.begin() + i);
					break;
				}
			}
		}

		// Remove entity from parent's child list
		else {
			std::vector<EntityId>& p_entities = m_EntityChildren[p_ent];
			size_t p_entities_size = p_entities.size();

			for (size_t i = 0; i < p_entities_size; i++) {
				if (p_entities[i] == eid) {
					p_entities.erase(p_entities.begin() + i);
					break;
				}
			}
		}

		// Callback for systems
		size_t systems_size = m_EntitySystems[eid].size();

		for (size_t i = 0; i < systems_size; i++) {
			System* system = m_EntitySystems[eid][i];
			system->Destroy();

			SystemHash s_hash = m_EntitySystemHashes[eid][i];

			std::vector<System*>& system_list = m_SystemsByHash[s_hash];

			size_t s_size = system_list.size();

			for (size_t i = 0; i < s_size; i++) {
				if (system_list[i] == system) {
					system_list.erase(system_list.begin() + i);
					break;
				}
			}

			delete system;
		}

		m_EntitySystemHashes[eid].clear();
		m_EntitySystems[eid].clear();

		// Add to removed component indexes
		std::map<ComponentId, size_t>::iterator c_it;

		for (c_it = m_EntityComponents[eid].begin(); c_it != m_EntityComponents[eid].end(); c_it++) {
			m_ComponentsRemoved[c_it->first].push_back(c_it->second);
		}

		m_EntityComponents[eid].clear();

		// Remove children entities
		size_t chsize = m_EntityChildren[eid].size();

		for (size_t i = 0; i < chsize; i++) {
			RemoveEntity(m_EntityChildren[eid][0]);
		}

		m_EntityChildren[eid].clear();
	}

	void EntityManager::UpdateChildTransforms(EntityId eid, Transform3D* t3dparent)
	{
		Transform3D* t3d = GetComponent<Transform3D>(eid);
		if (!t3d) return;
		// Update transforms
		t3d->position = t3dparent->position + t3d->localPosition;
		t3d->rotation = t3dparent->rotation + t3d->localRotation;
		t3d->scale = t3dparent->scale * t3d->localScale;

		// Calculate local matrix
		glm::mat4 rotation = glm::toMat4(glm::quat(glm::radians(t3d->localRotation)));
		t3d->localMatrix = glm::translate(glm::mat4(1.0f), t3d->localPosition) * rotation * glm::scale(glm::mat4(1.0f), t3d->localScale);
		// Calculate world matrix
		t3d->worldMatrix = t3dparent->worldMatrix * t3d->localMatrix;

		// Update children
		std::vector<EntityId>& children = m_EntityChildren[eid];
		size_t c_size = children.size();

		for (size_t i = 0; i < c_size; i++) {
			UpdateChildTransforms(children[i], t3d);
		}
	}

	void EntityManager::UpdateTransforms()
	{
		size_t p_ent_size = m_ParentEntitiesAlive.size();

		for (size_t i = 0; i < p_ent_size; i++) {
			EntityId p_ent = m_ParentEntitiesAlive[i];

			Transform3D* t3d = GetComponent<Transform3D>(p_ent);

			if (!t3d) return;

			t3d->position = t3d->localPosition;
			t3d->rotation = t3d->localRotation;
			t3d->scale = t3d->localScale;

			// Calculate local matrix
			glm::mat4 rotation = glm::toMat4(glm::quat(glm::radians(t3d->localRotation)));
			t3d->localMatrix = glm::translate(glm::mat4(1.0f), t3d->localPosition) * rotation * glm::scale(glm::mat4(1.0f), t3d->localScale);
			// Calculate world matrix
			t3d->worldMatrix = t3d->localMatrix;

			// Update children
			std::vector<EntityId>& children = m_EntityChildren[p_ent];
			size_t c_size = children.size();

			for (size_t i = 0; i < c_size; i++) {
				UpdateChildTransforms(children[i], t3d);
			}
		}
	}



	void EntityManager::SortParticleSystems()
	{
		std::map<SystemHash, std::vector<System*>>::iterator it = m_SystemsByHash.find(FNV1A_HASH("ParticleSystem"));

		//if no systems quit
		if (it == m_SystemsByHash.end())
			return;

		//get camera
		CameraManager& man = Wiwa::SceneManager::getActiveScene()->GetCameraManager();
		Camera* cam = man.getActiveCamera();
		
		if (cam == nullptr)
			return;

		// Sort from far to close
		std::sort(it->second.begin(), it->second.end(), [&](System* s1, System* s2) {
			Transform3D* t1 = GetComponent<Transform3D>(s1->GetEntity());
			Transform3D* t2 = GetComponent<Transform3D>(s2->GetEntity());

			float dist1 = Wiwa::Math::DistanceV3(t1->position, cam->getPosition());
			float dist2 = Wiwa::Math::DistanceV3(t2->position, cam->getPosition());

			return dist1 > dist2;
			});

		// Remove any null pointers from the vector
		it->second.erase(std::remove(it->second.begin(), it->second.end(), nullptr), it->second.end());
	}




	void EntityManager::SystemsAwake()
	{
		// Awake entity systems
		size_t entitySize = m_EntitySystems.size();

		for (size_t i = 0; i < entitySize; i++) {
			size_t system_size = m_EntitySystems[i].size();

			for (size_t j = 0; j < system_size; j++) {
				System* s = m_EntitySystems[i][j];

				s->Awake();
			}
		}
	}

	void EntityManager::SystemsInit()
	{
		// Init entity systems
		size_t entitySize = m_EntitySystems.size();

		for (size_t i = 0; i < entitySize; i++) {
			size_t system_size = m_EntitySystems[i].size();

			for (size_t j = 0; j < system_size; j++) {
				System* s = m_EntitySystems[i][j];

				s->Init();
			}
		}
	}

	void EntityManager::SystemsUpdate()
	{
		//// Update entity systems
		//size_t entitySize = m_EntitySystems.size();

		//for (size_t i = 0; i < entitySize; i++) {
		//	if (IsActive(i)) {
		//		size_t system_size = m_EntitySystems[i].size();

		//		for (size_t j = 0; j < system_size; j++) {
		//			System* s = m_EntitySystems[i][j];
		//			if (!s) continue;
		//			s->Update();
		//		}
		//	}
		//}


		SortParticleSystems();
		//update by systems

		// Update ozz animation systems second
		std::map<SystemHash, std::vector<System*>>::iterator ozz_animations = m_SystemsByHash.find(FNV1A_HASH("OzzAnimationSystem"));

		if (ozz_animations != m_SystemsByHash.end()) {
			std::vector<System*>& ozz_animation_list = ozz_animations->second;

			size_t size = ozz_animation_list.size();

			for (size_t i = 0; i < size; i++) {
				System* s = ozz_animation_list[i];

				if (!s) continue;

				s->Update();
			}
		}

		// Update mesh renderers first
		std::map<SystemHash, std::vector<System*>>::iterator meshes = m_SystemsByHash.find(FNV1A_HASH("MeshRenderer"));

		if (meshes != m_SystemsByHash.end()) {
			std::vector<System*>& mesh_list = meshes->second;

			size_t size = mesh_list.size();

			for (size_t i = 0; i < size; i++) {
				System* s = mesh_list[i];

				if (!s) continue;

				s->Update();
			}
		}

		

		

		// Update particle systems after previous renders
		std::map<SystemHash, std::vector<System*>>::iterator particles = m_SystemsByHash.find(FNV1A_HASH("ParticleSystem"));

		if (particles != m_SystemsByHash.end()) {
			std::vector<System*>& particles_list = particles->second;

			size_t size = particles_list.size();

			for (size_t i = 0; i < size; i++) {
				System* s = particles_list[i];

				if (!s) continue;

				s->Update();
			}
		}

		// Update the rest of systems
		std::map<SystemHash, std::vector<System*>>::iterator it;

		for (it = m_SystemsByHash.begin(); it != m_SystemsByHash.end(); it++)
		{
			if (it->first == FNV1A_HASH("MeshRenderer")) continue;
			if (it->first == FNV1A_HASH("ParticleSystem")) continue;
			if (it->first == FNV1A_HASH("OzzAnimationSystem")) continue;

			size_t system_size = it->second.size();
			for (size_t i = 0; i < system_size; i++)
			{
				System* s = it->second[i];
				
				if (!s) continue;

					s->Update();
			}
		}
	}
#include <glew.h>
	void EntityManager::Update()
	{
		// Apply systems in pool
		std::vector<System*> asd = m_SystemsToInit;

		m_SystemsToInit.clear();

		size_t ssize = asd.size();

		for (size_t i = 0; i < ssize; i++) {
			asd[i]->Awake();
		}

		for (size_t i = 0; i < ssize; i++) {
			asd[i]->Init();
		}

		// Remove entities in pool
		size_t rsize = m_EntitiesToDestroy.size();

		for (size_t i = 0; i < rsize; i++) {
			RemoveEntity(m_EntitiesToDestroy[i]);
		}

		m_EntitiesToDestroy.clear();

		// Update transforms
		UpdateTransforms();
	}

	void EntityManager::UpdateWhitelist()
	{
		size_t sw_size = m_SystemWhiteList.size();

		for (size_t i = 0; i < sw_size; i++) {
			std::vector<System*>& system_list = m_SystemsByHash[m_SystemWhiteList[i]];

			size_t s_count = system_list.size();

			for (size_t j = 0; j < s_count; j++) {
				if (IsActive(system_list[j]->GetEntity())) {
					system_list[j]->Update();
				}
			}
		}
	}

	void EntityManager::AddSystemToWhitelist(SystemHash system_hash)
	{
		if (IsWhitelistedSystem(system_hash)) return;
		if (!Application::Get().HasSystemH(system_hash)) return;

		m_SystemWhiteList.push_back(system_hash);
	}

	void EntityManager::AddSystemToWhitelist(const char* system_name)
	{
		AddSystemToWhitelist(FNV1A_HASH(system_name));
	}

	bool EntityManager::IsWhitelistedSystem(SystemHash system_hash)
	{
		size_t sw_size = m_SystemWhiteList.size();

		for (size_t i = 0; i < sw_size; i++) {
			if (m_SystemWhiteList[i] == system_hash) {
				return true;
			}
		}

		return false;
	}

	EntityId EntityManager::CreateEntity_impl()
	{
		size_t rsize = m_EntitiesRemoved.size();

		EntityId eid = 0;

		if (rsize > 0) {
			eid = m_EntitiesRemoved[rsize - 1];

			m_EntitiesRemoved.pop_back();
			m_EntityActive[eid] = true;
		}
		else {
			eid = m_EntityComponents.size();

			m_EntityComponents.emplace_back();
			m_EntitySystemHashes.emplace_back();
			m_EntitySystems.emplace_back();
			m_EntityNames.emplace_back();
			m_EntityParent.emplace_back();
			m_EntityChildren.emplace_back();
			m_EntityActive.emplace_back(true);
		}

		m_EntitiesAlive.push_back(eid);

		return eid;
	}

	EntityId EntityManager::CreateEntity()
	{
		return CreateEntity("New entity");
	}

	EntityId EntityManager::CreateEntity(const char* name)
	{
		EntityId eid = CreateEntity_impl();

		m_EntityNames[eid] = name;
		m_EntityParent[eid] = eid;

		m_ParentEntitiesAlive.emplace_back(eid);

		return eid;
	}

	EntityId EntityManager::CreateEntity(EntityId parent)
	{
		return CreateEntity("New entity", parent);
	}

	EntityId EntityManager::CreateEntity(const char* name, EntityId parent)
	{
		EntityId eid = CreateEntity_impl();

		m_EntityNames[eid] = name;
		m_EntityParent[eid] = parent;
		m_EntityChildren[parent].push_back(eid);

		return eid;
	}

	void EntityManager::SetParent(EntityId entity, EntityId parent)
	{
		EntityId prev_parent = m_EntityParent[entity];

		// If already is parent
		if (prev_parent == parent) return;

		// If entity didn't have a parent
		if (prev_parent == entity) {
			size_t p_size = m_ParentEntitiesAlive.size();

			for (size_t i = 0; i < p_size; i++) {
				if (m_ParentEntitiesAlive[i] == entity) {
					m_ParentEntitiesAlive.erase(m_ParentEntitiesAlive.begin() + i);
					break;
				}
			}
		}
		// If entity had a parent
		else {
			std::vector<EntityId>& prev_parent_children = m_EntityChildren[prev_parent];
			size_t size = prev_parent_children.size();

			for (size_t i = 0; i < size; i++) {
				if (prev_parent_children[i] == entity) {
					prev_parent_children.erase(prev_parent_children.begin() + i);
					break;
				}
			}
		}

		m_EntityParent[entity] = parent;

		if (entity == parent) {
			m_ParentEntitiesAlive.push_back(entity);
		}
		else {
			m_EntityChildren[parent].push_back(entity);
		}
	}

	void EntityManager::SavePrefab(EntityId entity, const char* path)
	{
		File file = Wiwa::FileSystem::Open(path, FileSystem::OM_OUT | FileSystem::OM_BINARY);

		if (file.IsOpen()) {
			_saveEntityImpl(file, entity);
		}

		file.Close();
	}

	EntityId EntityManager::LoadPrefab(const char* path)
	{

		std::filesystem::path assetspath = path;
		std::filesystem::path libpath = Wiwa::Resources::_assetToLibPath(path);

		if (!Wiwa::FileSystem::Exists(libpath.string().c_str()))
		{
			WI_CORE_CRITICAL("Prefab at {} doesn't exist", path);
			return WI_INVALID_INDEX;
		}

		File file = Wiwa::FileSystem::Open(libpath.string().c_str(), FileSystem::OM_IN | FileSystem::OM_BINARY);

		EntityId eid = WI_INVALID_INDEX;
		WI_CORE_INFO("Loading prefab {}", path);
		if (file.IsOpen()) {
			WI_CORE_INFO("Loaded prefab {}", path);
			eid = _loadEntityImpl(file, eid, true);
		}
		else
		{
			WI_CORE_CRITICAL("Can't load prefab {}", path);
		}
		file.Close();

		return eid;
	}

	EntityId EntityManager::LoadPrefab(const char* path, EntityId parent)
	{
		std::string filePath = Resources::_assetToLibPath(path);

		if (!Wiwa::FileSystem::Exists(filePath.c_str())) return WI_INVALID_INDEX;

		File file = Wiwa::FileSystem::Open(filePath.c_str(), FileSystem::OM_IN | FileSystem::OM_BINARY);

		EntityId eid = WI_INVALID_INDEX;

		if (file.IsOpen()) {
			eid = _loadEntityImpl(file, parent, true);
		}

		file.Close();

		return eid;
	}

	void EntityManager::DestroyEntity(EntityId entity)
	{
		m_EntitiesToDestroy.push_back(entity);
	}

	void EntityManager::ReserveEntities(size_t amount)
	{
		m_EntityComponents.reserve(amount);
		m_EntitySystemHashes.reserve(amount);
		m_EntityParent.reserve(amount);
		m_EntityChildren.reserve(amount);
		m_EntitiesAlive.reserve(amount);
		m_EntitiesRemoved.reserve(amount);
		m_ParentEntitiesAlive.reserve(amount);
		m_EntityActive.reserve(amount);
	}

	void EntityManager::ReserveComponent(ComponentHash hash, size_t amount)
	{
		// Get component ID
		ComponentId cid = GetComponentId(hash);
		const Type* c_type = m_ComponentIds[cid].ctype;
		size_t c_size = c_type->size;

		if (cid >= m_Components.size()) {
			m_Components.resize(cid + 1, NULL);
			m_ComponentsSize.resize(cid + 1, 0);
			m_ComponentsReserved.resize(cid + 1, 0);
			m_ComponentsRemoved.resize(cid + 1);
		}

		if (m_Components[cid]) {
			byte* oldBlock = m_Components[cid];
			size_t oldSize = m_ComponentsSize[cid] * c_size;
			size_t newSize = oldSize + amount * c_size;

			byte* newBlock = new byte[newSize];

			memcpy(newBlock, oldBlock, oldSize);

			delete[] oldBlock;

			m_Components[cid] = newBlock;
		}
		else {
			m_Components[cid] = new byte[amount * c_size];
		}

		m_ComponentsReserved[cid] = m_ComponentsSize[cid] + amount;
	}

	byte* EntityManager::AddComponent(EntityId entityId, ComponentHash hash, byte* value) {
		const Type* ctype = Application::Get().GetComponentTypeH(hash);

		return AddComponent(entityId, ctype, value);
	}

	byte* EntityManager::AddComponent(EntityId entity, const Type* type, byte* value)
	{
		if (type == NULL) return NULL;

		// Get component ID
		ComponentId cid = GetComponentId(type);

		// Take components map of the entity
		std::map<ComponentId, size_t>* ec = &m_EntityComponents[entity];

		byte* component = NULL;

		// Look if the entity has this component
		std::map<ComponentId, size_t>::iterator iterator = ec->find(cid);

		// If it doesn't have the component, add it
		if (iterator == ec->end()) {
			size_t t_size = type->size;
			// Check if the component list exists
			if (cid >= m_Components.size()) {
				m_Components.resize(cid + 1, NULL);
				m_ComponentsSize.resize(cid + 1, 0);
				m_ComponentsReserved.resize(cid + 1, 0);
				
				m_ComponentsRemoved.resize(cid + 1);
			}

			// If it's the first component, create new block MARTA WAS HERE
			if (!m_Components[cid]) {
				byte* block = new byte[t_size];

				component = block;//new(block) byte[t_size]{ 0 };

				if (value) memcpy(component, value, t_size);
				else memset(component, 0, t_size);

				m_Components[cid] = block;
				m_ComponentsSize[cid]++;
				m_ComponentsReserved[cid]++;
				ec->insert_or_assign(cid, 0);
			}
			else {
				// Check if component index available
				size_t c_rsize = m_ComponentsRemoved[cid].size();

				// If index is available, construct
				if (c_rsize > 0) {
					size_t c_index = m_ComponentsRemoved[cid][c_rsize - 1];

					byte* components = m_Components[cid];

					size_t ind = c_index * t_size;

					component = components + ind;//new(&components[ind]) byte[t_size]{ 0 };

					if (value) memcpy(component, value, t_size);
					else memset(component, 0, t_size);

					m_ComponentsRemoved[cid].pop_back();

					ec->insert_or_assign(cid, c_index);
				}
				// If not, proceed normally
				else {
					// If more components reserved, just construct me TOO MEEEEEEEEEEEEE :) :3 "^0^" ÙwÚ
					if (m_ComponentsSize[cid] < m_ComponentsReserved[cid]) {
						byte* components = m_Components[cid];

						size_t last = m_ComponentsSize[cid] * t_size;

						component = components + last;//new(&components[last]) byte[t_size]{ 0 };

						if (value) memcpy(component, value, t_size);
						else memset(component, 0, t_size);
					}
					// If not, expand the block and construct
					else {
						byte* oldBlock = m_Components[cid];
						size_t oldSize = m_ComponentsSize[cid] * t_size;
						size_t newSize = oldSize + t_size;

						byte* newBlock = new byte[newSize];

						memcpy(newBlock, oldBlock, oldSize);

						delete[] oldBlock;

						component = newBlock + oldSize;//new(&newBlock[oldSize]) byte[t_size]{ 0 };

						if (value) memcpy(component, value, t_size);
						else memset(component, 0, t_size);

						m_Components[cid] = newBlock;

						m_ComponentsReserved[cid]++;
					}

					// Update entity-component map and components size/reserved
					ec->insert_or_assign(cid, m_ComponentsSize[cid]);
					m_ComponentsSize[cid]++;
				}
			}
			OnComponentAdded(entity, component, type);
		}

		return component;
	}

	void EntityManager::RemoveComponent(EntityId entity, ComponentHash hash)
	{
		std::map<ComponentId, size_t>& c_map = m_EntityComponents[entity];

		std::map<ComponentId, size_t>::iterator c_it;

		for (c_it = c_map.begin(); c_it != c_map.end(); c_it++) {
			ComponentId c_id = c_it->first;
			const Type* c_type = m_ComponentTypes[c_id];
			
			if (c_type->hash == hash) {
				m_ComponentsRemoved[c_it->first].push_back(c_it->second);

				c_map.erase(c_it);

				OnComponentRemoved(entity, m_Components[c_it->first] + c_it->second * c_type->size, c_type);

				break;
			}
		}
	}

	void EntityManager::RemoveComponentById(EntityId entity, ComponentId componentId)
	{
		std::map<ComponentId, size_t>& c_map = m_EntityComponents[entity];
		std::map<ComponentId, size_t>::iterator c_it = c_map.find(componentId);

		if (c_it != c_map.end()) {
			m_ComponentsRemoved[c_it->first].push_back(c_it->second);

			const Type* c_type = m_ComponentTypes[c_it->first];

			OnComponentRemoved(entity, m_Components[c_it->first] + c_it->second * c_type->size, c_type);

			c_map.erase(c_it);
		}
	}

	byte* EntityManager::GetComponent(EntityId entityId, ComponentId componentId, size_t componentSize)
	{
		byte* c = NULL;

		if (entityId < m_EntityComponents.size()) {
			std::map<ComponentId, size_t>::iterator it = m_EntityComponents[entityId].find(componentId);

			if (it != m_EntityComponents[entityId].end()) {
				c = &m_Components[componentId][it->second * componentSize];
			}
		}

		return c;
	}

	byte* EntityManager::GetComponentByIterator(ComponentIterator c_it)
	{
		if (c_it.c_id == WI_INVALID_INDEX) return NULL;
		if (c_it.c_id >= m_ComponentIdCount) return NULL;
		if (c_it.c_index >= m_ComponentsSize[c_it.c_id]) return NULL;

		return m_Components[c_it.c_id] + c_it.c_index * c_it.c_size;
	}

	EntityManager::ComponentIterator EntityManager::GetComponentIterator(EntityId eid, ComponentHash c_hash)
	{
		ComponentIterator c_it = { WI_INVALID_INDEX, WI_INVALID_INDEX };

		ComponentId c_id = GetComponentId(c_hash);

		if (c_id == WI_INVALID_INDEX) return c_it;

		std::map<ComponentId, size_t>::iterator it = m_EntityComponents[eid].find(c_id);

		if (it == m_EntityComponents[eid].end()) return c_it;

		const Type* c_type = m_ComponentTypes[c_id];

		c_it.c_id = it->first;
		c_it.c_index = it->second;
		c_it.c_size = c_type->size;

		return c_it;
	}

	inline byte* EntityManager::GetComponents(ComponentId id, size_t* size)
	{
		*size = m_ComponentsSize[id];
		
		return m_Components[id];
	}

	inline byte* EntityManager::GetComponentsByHash(ComponentHash hash, size_t* size)
	{
		ComponentId cid = GetComponentId(hash);

		*size = m_ComponentsSize[cid];

		return m_Components[cid];
	}

	bool EntityManager::IsComponentRemoved(ComponentId id, size_t index)
	{
		std::vector<size_t>& ids = m_ComponentsRemoved[id];
		size_t s = ids.size();

		for (size_t i = 0; i < s; i++) {
			if (ids[i] == index) {
				return true;
			}
		}

		return false;
	}

	bool EntityManager::IsComponentRemovedByHash(ComponentHash hash, size_t index)
	{
		ComponentId cid = GetComponentId(hash);

		return IsComponentRemoved(cid, index);
	}

	EntityId EntityManager::GetEntityByName(const char* name)
	{
		size_t ecount = m_EntityNames.size();

		EntityId eid = WI_INVALID_INDEX;

		for (size_t i = 0; i < ecount; i++) {
			if (m_EntityNames[i] == name) {
				eid = i;
				break;
			}
		}

		return eid;
	}

	EntityId EntityManager::GetChildByName(EntityId parent, const char* name)
	{
		EntityId eid = WI_INVALID_INDEX;

		size_t ecount = m_EntityChildren[parent].size();

		for (size_t i = 0; i < ecount; i++) {
			EntityId id = m_EntityChildren[parent][i];

			if (m_EntityNames[id] == name) {
				eid = id;
				break;
			}

			eid = GetChildByName(id, name);

			if (eid != WI_INVALID_INDEX) {
				break;
			}
		}

		return eid;
	}

	size_t EntityManager::GetComponentIndex(EntityId entityId, ComponentId componentId, size_t componentSize)
	{
		size_t index = -1;

		if (entityId < m_EntityComponents.size()) {
			std::map<ComponentId, size_t>::iterator it = m_EntityComponents[entityId].find(componentId);

			if (it != m_EntityComponents[entityId].end()) {
				index = it->second;
			}
		}

		return index;
	}

	ComponentId EntityManager::GetComponentId(const Type* type)
	{
		if (!type) return WI_INVALID_INDEX;

		size_t component_id = WI_INVALID_INDEX;

		std::unordered_map<size_t, componentData>::iterator cid = m_ComponentIds.find(type->hash);

		if (cid == m_ComponentIds.end()) {
			component_id = m_ComponentIdCount++;

			m_ComponentIds[type->hash] = { type, component_id };

			if (m_ComponentTypes.size() >= component_id) {
				m_ComponentTypes.resize(component_id + 1, NULL);
			}
			
			m_ComponentTypes[component_id] = type;
		}
		else {
			component_id = cid->second.cid;
		}

		return component_id;
	}

	ComponentId EntityManager::GetComponentId(ComponentHash hash) {
		const Type* ctype = Application::Get().GetComponentTypeH(hash);

		return GetComponentId(ctype);
	}

	bool EntityManager::HasComponents(EntityId entityId, ComponentId* componentIds, size_t size)
	{
		bool hasComponents = true;

		if (entityId >= m_EntityComponents.size()) {
			return false;
		}

		std::map<ComponentId, size_t>* emap = &m_EntityComponents[entityId];

		for (size_t i = 0; i < size && hasComponents; i++) {
			hasComponents = hasComponents && emap->find(componentIds[i]) != emap->end();
		}

		return hasComponents;
	}

	bool EntityManager::HasSystem(EntityId eid, SystemHash system_hash)
	{
		size_t size = m_EntitySystemHashes[eid].size();

		for (size_t i = 0; i < size; i++) {
			if (m_EntitySystemHashes[eid][i] == system_hash)
				return true;
		}

		return false;
	}

	System* EntityManager::GetSystem(EntityId eid, SystemHash system_hash)
	{
		System* sys = NULL;

		size_t size = m_EntitySystemHashes[eid].size();

		for (size_t i = 0; i < size; i++) {
			if (m_EntitySystemHashes[eid][i] == system_hash)
			{
				sys = m_EntitySystems[eid][i];
				break;
			}
		}

		return sys;
	}

	size_t EntityManager::getSystemIndex(EntityId entityId, SystemHash system_hash)
	{
		size_t index = INVALID_INDEX;

		size_t size = m_EntitySystemHashes[entityId].size();

		for (size_t i = 0; i < size; i++) {
			if (m_EntitySystemHashes[entityId][i] == system_hash)
			{
				index = i;
				break;
			}
		}

		return index;
	}

	bool EntityManager::OnComponentAdded(EntityId entityId, byte* data, const Type* type)
	{
		if (SceneManager::isLoadingScene)
			return false;

		std::vector<System*>& systems = m_EntitySystems[entityId];
		size_t s_size = systems.size();

		//if (type->hash == (size_t)TypeHash::CollisionBody)
		//{
		//	Wiwa::CollisionBody* rigidBody = (Wiwa::CollisionBody*)data;
		//	rigidBody->positionOffset = { 0,0,0 };
		//	rigidBody->scalingOffset = { 1,1,1 };
		//	rigidBody->isTrigger = false;
		//	rigidBody->isStatic = false;
		//	rigidBody->doContinuousCollision = false;
		//	rigidBody->selfTag = 0;
		//	rigidBody->filterBits |= 1 << 0;
		//	//rigidBody->filterBits ^= (-0 ^ rigidBody->filterBits) & (1UL << 32);
		//}
		/*if (type->hash == (size_t)TypeHash::ColliderCube)
		{ 
			Wiwa::ColliderCube* colliderCube = (Wiwa::ColliderCube*)data;
			colliderCube->halfExtents= { 2,2,2 };
		}
		else if (type->hash == (size_t)TypeHash::ColliderSphere) 
		{
			Wiwa::ColliderSphere* colliderCube = (Wiwa::ColliderSphere*)data;
			colliderCube->radius = 1;
		}
		else if (type->hash == (size_t)TypeHash::ColliderCylinder) 
		{ 
			Wiwa::ColliderCylinder* colliderCube = (Wiwa::ColliderCylinder*)data;
			colliderCube->height = 1;
			colliderCube->radius = 1;
		}*/
		/*else if (type->hash == (size_t)TypeHash::ColliderCapsule)
		{
			Wiwa::ColliderCapsule* colliderCapsule = (Wiwa::ColliderCapsule*)data;
			colliderCapsule->height = 1;
			colliderCapsule->radius = 1;
		}*/
		/*else if (type->hash == (size_t)TypeHash::RayCast)
		{
			Wiwa::RayCast* rayCast = (Wiwa::RayCast*)data;
			rayCast->rayFromWorld = { 0,0,0 };
			rayCast->rayToWorld = { 0,0,0 };
			rayCast->doRayCasting = false;
		}*/

		for (size_t i = 0; i < s_size; i++) {
			systems[i]->OnComponentAdded(data, type);
		}

		return true;
	}

	bool EntityManager::OnComponentRemoved(EntityId entityId, byte* data, const Type* type)
	{
		std::vector<System*>& systems = m_EntitySystems[entityId];
		size_t s_size = systems.size();

		for (size_t i = 0; i < s_size; i++) {
			systems[i]->OnComponentRemoved(data, type);
		}

		return true;
	}

	System* EntityManager::_applySystemImpl(EntityId entityId, SystemHash system_hash)
	{
		const Type* stype = Application::Get().GetSystemTypeH(system_hash);

		System* system = NULL;

		if (stype) {
			system = (System*)stype->New();
			system->SetEntity(entityId);
			system->SetScene(m_Scene);
			system->SetTransform();
			system->OnSystemAdded();

			m_EntitySystems[entityId].push_back(system);
			m_EntitySystemHashes[entityId].push_back(system_hash);
			m_SystemsByHash[stype->hash].push_back(system);
		}
		else {
			WI_CORE_ERROR("System hash not found [{}]", system_hash);
		}

		return system;
	}

	void EntityManager::_saveEntityImpl(File& file, EntityId eid)
	{
		const char* e_name = GetEntityName(eid);
		size_t e_name_len = strlen(e_name) + 1;

		// Save entity name
		file.Write(&e_name_len, sizeof(size_t));
		file.Write(e_name, e_name_len);

		std::map<ComponentId, size_t>& components = GetEntityComponents(eid);
		std::map<ComponentId, size_t>::iterator c_it;
		size_t component_size = components.size();

		// Save component count for this entity
		file.Write(&component_size, sizeof(size_t));

		// For each component in entity
		for (c_it = components.begin(); c_it != components.end(); c_it++)
		{
			ComponentId c_id = c_it->first;
			const Type* c_type = GetComponentType(c_id);
			size_t c_size = c_type->size;
			byte* c_data = GetComponent(eid, c_id, c_size);

			// Save component hash, size and data
			file.Write(&c_type->hash, sizeof(size_t));
			file.Write(&c_size, sizeof(size_t));
			file.Write(c_data, c_size);
		}

		// Save entity systems
		const std::vector<SystemHash>& system_list = GetEntitySystemHashes(eid);
		size_t system_count = system_list.size();

		// Save system count
		file.Write(&system_count, sizeof(size_t));

		if (system_count > 0)
		{
			// Save system hashes
			file.Write(system_list.data(), system_count * sizeof(SystemHash));
		}

		// Check for child entities
		std::vector<EntityId>* children = GetEntityChildren(eid);
		size_t children_size = children->size();

		// Save children size (size >= 0)
		file.Write(&children_size, sizeof(size_t));

		// Recursively save each child entity
		for (size_t i = 0; i < children_size; i++)
		{
			_saveEntityImpl(file, children->at(i));
		}
	}

	EntityId EntityManager::_loadEntityImpl(File& file, EntityId parent, bool is_parent)
	{
		size_t e_name_len;
		char* e_name_c;
		std::string e_name;

		// Read entity name
		file.Read(&e_name_len, sizeof(size_t));
		e_name_c = new char[e_name_len];
		file.Read(e_name_c, e_name_len);
		e_name = e_name_c;
		delete[] e_name_c;

		EntityId eid;

		if (is_parent)
		{
			eid = CreateEntity(e_name.c_str());
		}
		else
		{
			eid = CreateEntity(e_name.c_str(), parent);
		}

		size_t component_size;

		// Read component count
		file.Read(&component_size, sizeof(size_t));

		// For each component in entity
		for (size_t i = 0; i < component_size; i++)
		{
			ComponentHash c_hash;
			size_t c_size;
			byte* data;

			// Read component hash, size and data
			file.Read(&c_hash, sizeof(size_t));
			file.Read(&c_size, sizeof(size_t));
			data = new byte[c_size];
			file.Read(data, c_size);
			//WI_INFO("Loading component {}", Application::Get().GetComponentTypeH(c_hash)->name.c_str());
			byte* component = AddComponent(eid, c_hash, data);
			delete[] data;

			size_t mesh_hash = GetComponentType(GetComponentId<Mesh>())->hash;

			if (c_hash == mesh_hash)
			{
				Mesh* mesh = (Mesh*)component;

				size_t meshpath_size = strlen(mesh->mesh_path);
				if (meshpath_size > 0)
					mesh->meshId = Resources::Load<Model>(mesh->mesh_path);

				size_t matpath_size = strlen(mesh->mat_path);
				if (matpath_size > 0)
					mesh->materialId = Resources::Load<Material>(mesh->mat_path);
			}
		}

		size_t system_count;

		// Read system count
		file.Read(&system_count, sizeof(size_t));

		if (system_count > 0)
		{
			SystemHash* system_hashes = new SystemHash[system_count];

			// Read system hashes
			file.Read(system_hashes, system_count * sizeof(SystemHash));

			for (size_t i = 0; i < system_count; i++)
			{
				ApplySystem(eid, system_hashes[i]);
			}
		}

		// Check for child entities
		size_t children_size;

		// Read children size (size >= 0)
		file.Read(&children_size, sizeof(size_t));

		for (size_t i = 0; i < children_size; i++)
		{
			_loadEntityImpl(file, eid, false);
		}

		return eid;
	}

	void EntityManager::ApplySystem(EntityId eid, SystemHash system_hash)
	{
		if (HasSystem(eid, system_hash)) return;

		System* sys = _applySystemImpl(eid, system_hash);

		if (m_InitSystemsOnApply && sys) {
			m_SystemsToInit.push_back(sys);
		}
	}

	void EntityManager::ApplySystem(EntityId eid, const Type* system_type)
	{
		ApplySystem(eid, system_type->hash);
	}

	void EntityManager::RemoveSystem(EntityId eid, SystemHash system_hash)
	{
		size_t sindex = getSystemIndex(eid, system_hash);

		if (sindex == INVALID_INDEX) return;

		System* system = m_EntitySystems[eid][sindex];

		m_EntitySystems[eid].erase(m_EntitySystems[eid].begin() + sindex);
		m_EntitySystemHashes[eid].erase(m_EntitySystemHashes[eid].begin() + sindex);

		std::vector<System*>& sys = m_SystemsByHash[system_hash];
		size_t size = sys.size();

		for (size_t i = 0; i < size; i++){
			if (sys[i] == system) {
				sys.erase(sys.begin() + i);
				break;
			}
		}

		system->OnSystemRemoved();
		delete system;
	}
}