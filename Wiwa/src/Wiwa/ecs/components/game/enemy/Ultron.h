#pragma once
#pragma warning(disable : 4302)
#pragma warning(disable : 4311)
#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/Reflection.h>
#include <Wiwa/utilities/math/Vector3f.h>

#include <glm/glm.hpp>
namespace Wiwa {
	struct WI_API Ultron {
		/*-------- Bullet Storm --------*/
		// Splash
		int bulletStorm_firstPattern_bulletsPerWave = 8;
		int bulletStorm_firstPattern_waves = 4;

		// Spiral
		int bulletStorm_secondPattern_bulletsPerSpiral = 8;
		float bulletStorm_secondPattern_timerBetweenSpirals = 0.2f;
		int bulletStorm_secondPattern_spirals = 3;

		// Cross
		int bulletStorm_thirdPattern_bulletsPerCross = 8;
		float bulletStorm_thirdPattern_timerBetweenCross = 0.2f;
		int bulletStorm_thirdPattern_crosses = 3;

		/*-------- Cluster Shot --------*/
		int clusterShot_bullets = 3;
		float clusterShot_timerBetweenBullets = 2.0f;
		int clusterShot_miniBullets = 8;

		/*-------- Laser beam --------*/
		int laserBeam_tickDamage = 20;
		float laserBeam_tickTimer = 1.0f;
		int laserBeam_duration = 5;

		float bulletSpeed;
		float bulletLifeTime;
	};
}

REFLECTION_BEGIN(Wiwa::Ultron)
REFLECTION_END;