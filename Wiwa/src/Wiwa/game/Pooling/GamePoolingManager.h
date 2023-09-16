#pragma once
#include <Wiwa/utilities/EntityPool.h>

namespace Wiwa
{
    class Scene;
    class WI_API GamePoolingManager {
    public:
        GamePoolingManager();
        ~GamePoolingManager();
        EntityPool* s_PhalanxMeleeGenericPool;
        EntityPool* s_PhalanxMeleeVariantAPool;
        EntityPool* s_PhalanxMeleeVariantBPool;

        EntityPool* s_PhalanxRangedGenericPool;
        EntityPool* s_PhalanxRangedVariantAPool;
        EntityPool* s_PhalanxRangedVariantBPool;

        EntityPool* s_SentinelPool;
        EntityPool* s_SubjugatorPool;
        EntityPool* s_SubjugatorChiefPool;

        EntityPool* s_SimpleBulletsPool;
        EntityPool* s_StarLordBullets;
        EntityPool* s_RocketBullets;
        EntityPool* s_ZigZagBulletPool;
        EntityPool* s_StarHawksBlastPool;

        EntityPool* s_SentinelExplosion;
        EntityPool* s_ExplosiveBarrel;

        EntityPool* s_ClusterBulletsPool;
        EntityPool* s_UltronLaserBeamPool;
        EntityPool* s_UltronSecondDashPool;
        EntityPool* s_RainProjectilePool;
        EntityPool* s_SmashExplosionPool;

        bool m_HasLoadedAll = false;
        void SetScene(Scene* scene);

        void LoadPool(Pool_Type type, Scene* scene);
        void UnloadPool(Pool_Type type);

        void LoadAllPools(Scene* scene);
        void UnloadAllPools();
    };
}