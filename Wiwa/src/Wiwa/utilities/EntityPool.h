#pragma once
#include <queue>
#include <memory>
#include <vector>
#include <string>

typedef size_t EntityId;

namespace Wiwa
{
    enum Pool_Type
    {
        PHALANX_MELEE_GENERIC = 0,
        PHALANX_MELEE_VARIANT_A,
        PHALANX_MELEE_VARIANT_B,

        PHALANX_RANGED_GENERIC,
        PHALANX_RANGED_VARIANT_A,
        PHALANX_RANGED_VARIANT_B,

        SENTINEL,

        SUBJUGATOR,
        SUBJUGATOR_CHIEF,

        SIMPLE_BULLET,
        SENTINEL_EXPLOSION,
        CLUSTER_BULLET,
        ULTRON_LASER_BEAM,
        EXPLOSIVE_BARREL,
        ULTRON_SECOND_DASH,
        RAIN_PROJECTILE,
        ULTRON_SMASH_EXPLOSION,
        ZIG_ZAG_BULLET,

        STARLORD_BULLET,
        ROCKET_BULLET,

        STARHAWK_BLAST_BULLET,

        MAX_POOL
    };

    class Scene;
    class WI_API EntityPool {
    private:
        std::queue<EntityId> m_ActiveEntities;
        std::queue<EntityId> m_DisabledEntities;
        Scene* m_Scene;
        std::string m_Path;
        int m_MaxSize;
        Pool_Type m_Type;
    public:
        EntityPool(Pool_Type type, int max_size, const char* path)
            : m_Type(type), m_MaxSize(max_size), m_Path(path), Loaded(false) {};
        ~EntityPool();

        EntityId GetFromPool();

        void ReturnToPool(EntityId entity_id);

        void IncreasePoolSize(const std::vector<EntityId>& new_entities, bool disable = true);

        void LoadPool(Scene* scene);

        void ReleasePool();

        void SetScene(Scene* scene);

        inline const char* getPath() { return m_Path.c_str(); };

        inline int getMaxSize() { return m_MaxSize; };

        inline int getCountActive() { return (int)m_ActiveEntities.size(); };

        inline int getCountDisabled() { return (int)m_DisabledEntities.size(); };

        inline bool hasDisabled() { if (getCountDisabled() > 0) return true; else return false; }

        inline Pool_Type getType() { return m_Type; };

        inline  std::queue<EntityId> getDisabledEntities() { return m_DisabledEntities; };

        bool Loaded = false;
    };
}

