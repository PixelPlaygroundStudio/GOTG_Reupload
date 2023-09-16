#include <wipch.h>
#include "EntityPool.h"
#include <Wiwa/ecs/EntityManager.h>
#include <Wiwa/scene/SceneManager.h>
#include <Wiwa/ecs/systems/System.h>
namespace Wiwa
{
    EntityPool::~EntityPool()
    {
        ReleasePool();
    }

    EntityId EntityPool::GetFromPool()
    {
       /* if (!m_Loaded)
            GameStateManager::s_PoolManager->LoadPool(m_Type, m_Scene);*/

        // The pool type doesn't exist, create it callin from IncreasePoolSize();
        if (m_DisabledEntities.empty()) {
            WI_INFO("Pool {} is empty", (int)getType());
            return EntityManager::INVALID_INDEX;
        }

        // Pop the first from the queue
        EntityId entityId = m_DisabledEntities.front();
        m_DisabledEntities.pop();
        m_ActiveEntities.push(entityId);

        EntityManager&em = m_Scene->GetEntityManager();
        em.SetActive(entityId, true);

        // Enable the required systems
        std::vector<System*> systems = em.GetEntitySystems(entityId);
        size_t entitySystemsSize = systems.size();

        for (size_t i = 0; i < entitySystemsSize; i++) {
            System* s = systems[i];
            if (!s) continue;
            s->OnEnabledFromPool();
        }

        return entityId;
    }

    void EntityPool::ReturnToPool(EntityId entity_id)
    {
        EntityManager& em = m_Scene->GetEntityManager();

        em.SetActive(entity_id, false);

        m_DisabledEntities.push(entity_id);
        m_ActiveEntities.pop();

        // Disable the required systems
        std::vector<System*> systems = em.GetEntitySystems(entity_id);
        size_t entitySystemsSize = systems.size();

        for (size_t i = 0; i < entitySystemsSize; i++) {
            System* s = systems[i];
            if (!s) continue;
            s->OnDisabledFromPool();
        }
    }

    void EntityPool::IncreasePoolSize(const std::vector<EntityId>& new_entities, bool disable)
    {
        if (Loaded)
            return;

        EntityManager& em = m_Scene->GetEntityManager();

        size_t size = new_entities.size();
        for (size_t i = 0; i < size; i++) 
        {
            // Add the entity id to the pool
            EntityId id = new_entities[i];
            m_DisabledEntities.push(id);

            em.SetActive(id, false);

            if (!disable)
                continue;

            std::vector<System*> systems = em.GetEntitySystems(id);
            size_t entitySystemsSize = systems.size();

            for (size_t j = 0; j < entitySystemsSize; j++) {
                System* s = systems[j];
                if (!s) continue;
                s->OnDisabledFromPool();
            }
        }

        Loaded = true;
    }

    void EntityPool::LoadPool(Scene* scene)
    {
        if (Loaded)
            return;
        SetScene(scene);
        std::vector<EntityId> ids(getMaxSize());
        for (int i = 0; i < getMaxSize(); i++)
            ids[i] = scene->GetEntityManager().LoadPrefab(getPath());
       IncreasePoolSize(ids);
    }

    void EntityPool::ReleasePool()
    {
        if (!Loaded)
            return;

        EntityManager& em = m_Scene->GetEntityManager();
        size_t size = m_DisabledEntities.size();
        for (size_t i = 0; i < size; i++)
        {
            m_DisabledEntities.pop();
        }

        Loaded = false;
    }

    void EntityPool::SetScene(Scene* scene)
    {
        m_Scene = scene;
    }


}
