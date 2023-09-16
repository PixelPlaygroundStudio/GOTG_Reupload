#include <wipch.h>
#include "GamePoolingManager.h"
#include <Wiwa/utilities/EntityPool.h>
#include <Wiwa/ecs/EntityManager.h>
#include <Wiwa/scene/SceneManager.h>

namespace Wiwa
{
	GamePoolingManager::GamePoolingManager()
	{
		s_PhalanxMeleeGenericPool = new EntityPool(Pool_Type::PHALANX_MELEE_GENERIC, 3, "assets\\Enemy\\PhalanxMelee\\MeleePhalanx_01.wiprefab");
		s_PhalanxMeleeVariantAPool = new EntityPool(Pool_Type::PHALANX_MELEE_VARIANT_A, 3, "assets\\Enemy\\PhalanxMelee\\MeleePhalanx_01a.wiprefab");
		s_PhalanxMeleeVariantBPool = new EntityPool(Pool_Type::PHALANX_MELEE_VARIANT_B, 3, "assets\\Enemy\\PhalanxMelee\\MeleePhalanx_01b.wiprefab");

		s_PhalanxRangedGenericPool = new EntityPool(Pool_Type::PHALANX_RANGED_GENERIC, 3, "assets\\Enemy\\PhalanxRanged\\RangedPhalanx_01.wiprefab");
		s_PhalanxRangedVariantAPool = new EntityPool(Pool_Type::PHALANX_MELEE_VARIANT_A, 3, "assets\\Enemy\\PhalanxRanged\\RangedPhalanx_01a.wiprefab");
		s_PhalanxRangedVariantBPool = new EntityPool(Pool_Type::PHALANX_MELEE_VARIANT_B, 3, "assets\\Enemy\\PhalanxRanged\\RangedPhalanx_01b.wiprefab");

		s_SentinelPool = new EntityPool(Pool_Type::SENTINEL, 3, "assets\\Enemy\\Sentinel\\sk_sentinel01a.wiprefab");
		s_SubjugatorPool = new EntityPool(Pool_Type::SUBJUGATOR, 6, "assets\\Enemy\\Subjugator\\sk_subjugator01.wiprefab");
		s_SubjugatorChiefPool = new EntityPool(Pool_Type::SUBJUGATOR, 6, "assets\\Enemy\\Subjugator\\sk_subjugator01_chief.wiprefab");

		s_SimpleBulletsPool = new EntityPool(Pool_Type::SIMPLE_BULLET, 100, "assets\\Enemy\\SimpleBullet\\SimpleBullet_01.wiprefab");
		s_SentinelExplosion = new EntityPool(Pool_Type::SENTINEL_EXPLOSION, 3, "assets\\Enemy\\Explosions\\SentinelExplosion_01.wiprefab");
		s_ClusterBulletsPool = new EntityPool(Pool_Type::CLUSTER_BULLET,12, "assets\\Enemy\\ClusterBullet\\ClusterBullet_01.wiprefab");
		s_UltronLaserBeamPool = new EntityPool(Pool_Type::ULTRON_LASER_BEAM, 1, "assets\\Enemy\\UltronLaserBeam\\UltronLaserBeam_01.wiprefab");
		s_ExplosiveBarrel = new EntityPool(Pool_Type::EXPLOSIVE_BARREL, 6, "assets\\Enemy\\Explosions\\ExplosiveBarrelExplosion_01.wiprefab");
		s_StarLordBullets = new EntityPool(Pool_Type::STARLORD_BULLET, 25, "assets\\Prefabs\\Bullet\\P_StarLordBullet.wiprefab");
		s_RocketBullets = new EntityPool(Pool_Type::ROCKET_BULLET, 2, "assets\\Prefabs\\Bullet\\P_StarLordBullet.wiprefab");
		s_UltronSecondDashPool = new EntityPool(Pool_Type::ULTRON_SECOND_DASH, 1, "assets\\Enemy\\DashEffect\\DashEffect_01.wiprefab");
		s_RainProjectilePool = new EntityPool(Pool_Type::RAIN_PROJECTILE, 80, "assets\\Enemy\\RainProjectile\\RainProjectile_01.wiprefab");
		s_SmashExplosionPool = new EntityPool(Pool_Type::ULTRON_SMASH_EXPLOSION, 12, "assets\\Enemy\\Explosions\\SmashExplosion_01.wiprefab");
		s_ZigZagBulletPool = new EntityPool(Pool_Type::ZIG_ZAG_BULLET, 40, "assets\\Enemy\\ZigZagBullet\\ZigZagBullet_01.wiprefab");
		s_StarHawksBlastPool = new EntityPool(Pool_Type::STARHAWK_BLAST_BULLET, 40, "assets\\Prefabs\\AbilitiesPrefabs\\P_StarHawkBlast.wiprefab");
		m_HasLoadedAll = false;
	}

	GamePoolingManager::~GamePoolingManager()
	{
		delete s_PhalanxMeleeGenericPool;
		delete s_PhalanxMeleeVariantAPool;
		delete s_PhalanxMeleeVariantBPool;

		delete s_PhalanxRangedGenericPool;
		delete s_PhalanxRangedVariantAPool;
		delete s_PhalanxRangedVariantBPool;

		delete s_SentinelPool;	
		delete s_SubjugatorPool;
		delete s_SubjugatorChiefPool;

		delete s_SimpleBulletsPool;
		delete s_SentinelExplosion;
		delete s_ClusterBulletsPool;
		delete s_UltronLaserBeamPool;
		delete s_ExplosiveBarrel;
		delete s_UltronSecondDashPool;
		delete s_RainProjectilePool;
		delete s_SmashExplosionPool;
		delete s_ZigZagBulletPool;

		delete s_StarHawksBlastPool;
	}

	void GamePoolingManager::SetScene(Scene* scene)
	{
		s_PhalanxMeleeGenericPool->SetScene(scene);
		s_PhalanxMeleeVariantAPool->SetScene(scene);
		s_PhalanxMeleeVariantBPool->SetScene(scene);

		s_PhalanxRangedGenericPool->SetScene(scene);
		s_PhalanxRangedVariantAPool->SetScene(scene);
		s_PhalanxRangedVariantBPool->SetScene(scene);

		s_SentinelPool->SetScene(scene);
		s_SubjugatorPool->SetScene(scene);
		s_SubjugatorChiefPool->SetScene(scene);

		s_SimpleBulletsPool->SetScene(scene);
		s_SentinelExplosion->SetScene(scene);
		s_ClusterBulletsPool->SetScene(scene);
		s_UltronLaserBeamPool->SetScene(scene);
		s_ExplosiveBarrel->SetScene(scene);
		s_UltronSecondDashPool->SetScene(scene);
		s_RainProjectilePool->SetScene(scene);
		s_SmashExplosionPool->SetScene(scene);
		s_ZigZagBulletPool->SetScene(scene);
		s_StarHawksBlastPool->SetScene(scene);
	}


	void GamePoolingManager::LoadPool(Pool_Type type, Scene* scene)
	{
		switch (type)
		{
		case Pool_Type::PHALANX_MELEE_GENERIC:
			s_PhalanxMeleeGenericPool->LoadPool(scene);
			break;
		case Pool_Type::PHALANX_MELEE_VARIANT_A:
			s_PhalanxMeleeVariantAPool->LoadPool(scene);
			break;
		case Pool_Type::PHALANX_MELEE_VARIANT_B:
			s_PhalanxMeleeVariantBPool->LoadPool(scene);
			break;
		case Pool_Type::PHALANX_RANGED_GENERIC:
			s_PhalanxRangedGenericPool->LoadPool(scene);
			break;
		case Pool_Type::PHALANX_RANGED_VARIANT_A:
			s_PhalanxRangedVariantAPool->LoadPool(scene);
			break;
		case Pool_Type::PHALANX_RANGED_VARIANT_B:
			s_PhalanxRangedVariantBPool->LoadPool(scene);
			break;
		case Pool_Type::SENTINEL:
			s_SentinelPool->LoadPool(scene);
			break;
		case Pool_Type::SUBJUGATOR:
			s_SubjugatorPool->LoadPool(scene);
			break;
		case Pool_Type::SUBJUGATOR_CHIEF:
			s_SubjugatorChiefPool->LoadPool(scene);
			break;
		case Pool_Type::SIMPLE_BULLET:
			s_SimpleBulletsPool->LoadPool(scene);
			break;
		case Pool_Type::SENTINEL_EXPLOSION:
			s_SentinelExplosion->LoadPool(scene);
			break;
		case Pool_Type::CLUSTER_BULLET:
			s_ClusterBulletsPool->LoadPool(scene);
			break;
		case Pool_Type::ULTRON_LASER_BEAM:
			s_UltronLaserBeamPool->LoadPool(scene);
			break;
		case Pool_Type::EXPLOSIVE_BARREL:
			s_ExplosiveBarrel->LoadPool(scene);
			break;
		case Pool_Type::STARLORD_BULLET:
			s_StarLordBullets->LoadPool(scene);
			break;
		case Pool_Type::ROCKET_BULLET:
			s_RocketBullets->LoadPool(scene);
			break;
		case Pool_Type::ULTRON_SECOND_DASH:
			s_UltronSecondDashPool->LoadPool(scene);
			break;
		case Pool_Type::RAIN_PROJECTILE:
			s_RainProjectilePool->LoadPool(scene);
			break;
		case Pool_Type::ULTRON_SMASH_EXPLOSION:
			s_SmashExplosionPool->LoadPool(scene);
			break;
		case Pool_Type::ZIG_ZAG_BULLET:
			s_ZigZagBulletPool->LoadPool(scene);
			break;
		case Pool_Type::STARHAWK_BLAST_BULLET:
			s_StarHawksBlastPool->LoadPool(scene);
			break;
		default:
			break;
		}
	}
	void GamePoolingManager::UnloadPool(Pool_Type type)
	{
		switch (type)
		{
		case Pool_Type::PHALANX_MELEE_GENERIC:
			s_PhalanxMeleeGenericPool->ReleasePool();
			break;
		case Pool_Type::PHALANX_MELEE_VARIANT_A:
			s_PhalanxMeleeVariantAPool->ReleasePool();
			break;
		case Pool_Type::PHALANX_MELEE_VARIANT_B:
			s_PhalanxMeleeVariantBPool->ReleasePool();
			break;
		case Pool_Type::PHALANX_RANGED_GENERIC:
			s_PhalanxRangedGenericPool->ReleasePool();
			break;
		case Pool_Type::PHALANX_RANGED_VARIANT_A:
			s_PhalanxRangedVariantAPool->ReleasePool();
			break;
		case Pool_Type::PHALANX_RANGED_VARIANT_B:
			s_PhalanxRangedVariantBPool->ReleasePool();
			break;
		case Pool_Type::SENTINEL:
			s_SentinelPool->ReleasePool();
			break;
		case Pool_Type::SUBJUGATOR:
			s_SubjugatorPool->ReleasePool();
			break;
		case Pool_Type::SUBJUGATOR_CHIEF:
			s_SubjugatorChiefPool->ReleasePool();
			break;
		case Pool_Type::SIMPLE_BULLET:
			s_SimpleBulletsPool->ReleasePool();
			break;
		case Pool_Type::SENTINEL_EXPLOSION:
			s_SentinelExplosion->ReleasePool();
			break;
		case Pool_Type::CLUSTER_BULLET:
			s_ClusterBulletsPool->ReleasePool();
			break;
		case Pool_Type::ULTRON_LASER_BEAM:
			s_UltronLaserBeamPool->ReleasePool();
			break;
		case Pool_Type::EXPLOSIVE_BARREL:
			s_ExplosiveBarrel->ReleasePool();
			break;
		case Pool_Type::STARLORD_BULLET:
			s_StarLordBullets->ReleasePool();
			break;
		case Pool_Type::ROCKET_BULLET:
			s_RocketBullets->ReleasePool();
			break;
		case Pool_Type::ULTRON_SECOND_DASH:
			s_UltronSecondDashPool->ReleasePool();
			break;
		case Pool_Type::RAIN_PROJECTILE:
			s_RainProjectilePool->ReleasePool();
			break;
		case Pool_Type::ULTRON_SMASH_EXPLOSION:
			s_SmashExplosionPool->ReleasePool();
			break;
		case Pool_Type::ZIG_ZAG_BULLET:
			s_ZigZagBulletPool->ReleasePool();
			break;
		case Pool_Type::STARHAWK_BLAST_BULLET:
			s_StarHawksBlastPool->ReleasePool();
			break;
		default:
			break;
		}
	}

	void GamePoolingManager::LoadAllPools(Scene* scene)
	{
		if (m_HasLoadedAll)
			return;

		s_PhalanxMeleeGenericPool->LoadPool(scene);
		s_PhalanxMeleeVariantAPool->LoadPool(scene);
		s_PhalanxMeleeVariantBPool->LoadPool(scene);

		s_PhalanxRangedGenericPool->LoadPool(scene);
		s_PhalanxRangedVariantAPool->LoadPool(scene);
		s_PhalanxRangedVariantBPool->LoadPool(scene);

		s_SentinelPool->LoadPool(scene);
		s_SubjugatorPool->LoadPool(scene);
		s_SubjugatorChiefPool->LoadPool(scene);

		s_SimpleBulletsPool->LoadPool(scene);
		s_SentinelExplosion->LoadPool(scene);
		s_ClusterBulletsPool->LoadPool(scene);
		s_UltronLaserBeamPool->LoadPool(scene);
		s_ExplosiveBarrel->LoadPool(scene);
		s_UltronSecondDashPool->LoadPool(scene);
		s_RainProjectilePool->LoadPool(scene);
		s_SmashExplosionPool->LoadPool(scene);
		s_ZigZagBulletPool->LoadPool(scene);

		s_StarHawksBlastPool->LoadPool(scene);

		m_HasLoadedAll = true;
	}
	void GamePoolingManager::UnloadAllPools()
	{
		m_HasLoadedAll = false;

		s_PhalanxMeleeGenericPool->ReleasePool();
		s_PhalanxMeleeVariantAPool->ReleasePool();
		s_PhalanxMeleeVariantBPool->ReleasePool();

		s_PhalanxRangedGenericPool->ReleasePool();
		s_PhalanxRangedVariantAPool->ReleasePool();
		s_PhalanxRangedVariantBPool->ReleasePool();

		s_SentinelPool->ReleasePool();
		s_SubjugatorPool->ReleasePool();
		s_SubjugatorChiefPool->ReleasePool();

		s_SimpleBulletsPool->ReleasePool();
		s_SentinelExplosion->ReleasePool();
		s_ClusterBulletsPool->ReleasePool();
		s_UltronLaserBeamPool->ReleasePool();
		s_ExplosiveBarrel->ReleasePool();
		s_UltronSecondDashPool->ReleasePool();
		s_RainProjectilePool->ReleasePool();
		s_SmashExplosionPool->ReleasePool();
		s_ZigZagBulletPool->ReleasePool();

		s_StarHawksBlastPool->ReleasePool();

	}
}