#pragma once
#pragma warning(disable : 4302)
#include <Wiwa/core/Core.h>
#include "Wiwa/ecs/EntityManager.h"
#include <glm/glm.hpp>
#include <Wiwa/utilities/Reflection.h>
#include <Wiwa/core/Resources.h>

namespace Wiwa {


	enum ParticleSpawnVolume
	{
		NONE = 0, CUBE = 1, SPHERE = 2
	};

	struct ColorNode
	{
		float m_percentage;
		glm::vec4 color;
		ColorNode()
		{
			m_percentage = 0.f;
			color = glm::vec4(1, 1, 1, 1);
		}
	};

	struct WI_API ParticleEmitterComponent {

		//Emitter data			----------------------------------------
		
		

		ResourceId m_materialId;
		char m_materialPath[128];

		ResourceId m_mesh;
		char m_meshPath[128];

		bool m_meshChanged;
		bool m_materialChanged;

		
		int m_maxParticles;

		int m_activeParticles;

		//spawning
		bool m_loopSpawning;
		float m_spawnRate;
		
		bool m_rangedSpawnAmount;
		int  m_spawnAmount;
		int  m_minSpawnAmount;
		int  m_maxSpawnAmount;


		bool	m_p_rangedSpawnRate;
		float	m_p_minSpawnRate;
		float	m_p_maxSpawnRate;
		//float m_spawnDelay;

		//if it starts counting down from start (if using active over time)
		bool	m_startActive;
		bool	m_active;
		bool	m_activeTimeChanged;
		bool	m_rangedTimeActive;
		bool	m_activeOverTime;
		float	m_ActiveTimer;
		float	m_initialTimeActive;
		float	m_minInitialTimeActive;
		float	m_maxInitialTimeActive;

		ParticleSpawnVolume m_spawnVolume;

		EntityId emitterOwner;
		//Particle Data			----------------------------------------

		//general data
		float	m_p_lifeTime;
		bool	m_p_rangedLifeTime;
		float	m_p_minLifeTime;
		float	m_p_maxLifeTime;


		bool m_billboardActive;


		//transform data


		//position & translation
		bool		m_p_followEmitterPosition;
		bool		m_p_followEmitterPositionSpawn;
		glm::vec3	m_p_initialPosition;
		glm::vec3	m_p_initialPositionSphCenter;
		float		m_p_initialPositionSphRadius;
		glm::vec3	m_p_initialPositionBoxA;
		glm::vec3	m_p_initialPositionBoxB;
		bool		m_p_positionTowardsPoint;
		glm::vec3	m_p_positionTowardsPointPos;
		float		m_p_translationTime;

		glm::vec3	m_p_initialVelocity;
		bool		m_p_rangedVelocity;
		glm::vec3	m_p_minVelocity;
		glm::vec3	m_p_maxVelocity;

		bool m_p_useGravity;
		float m_p_gravity;

		//rotation
		
		bool		m_p_followEmitterRotation;
		bool		m_p_followEmitterRotationSpawn;
		glm::vec3	m_p_initialRotation;
		bool		m_p_rangedInitialRotation;
		glm::vec3	m_p_minInitialRotation;
		glm::vec3	m_p_maxInitialRotation;

		glm::vec3	m_p_initialAngularVelocity;
		bool		m_p_rangedAngularVelocity;
		glm::vec3	m_p_minInitialAngularVelocity;
		glm::vec3	m_p_maxInitialAngularVelocity;
		float		m_p_rotateTime;

		bool		m_p_rotationOverTime;
		float		m_p_rotationOverTimePerStart;
		float		m_p_rotationOverTimePerEnd;
		glm::vec3	m_p_rotationOverTimeStart;
		glm::vec3	m_p_rotationOverTimeEnd;
		


		glm::vec3	m_p_initialScale;
		bool		m_p_rangedInitialScale;
		glm::vec3	m_p_minInitialScale;
		glm::vec3	m_p_maxInitialScale;

		bool		m_p_scaleOverTime;
		float		m_p_scaleOverTimePerStart;
		float		m_p_scaleOverTimePerEnd;
		glm::vec3	m_p_scaleOverTimeStart;
		glm::vec3	m_p_scaleOverTimeEnd;

		glm::vec3	m_p_initialGrowthVelocity;
		bool		m_p_rangedGrowthVelocity;
		glm::vec3	m_p_minInitialGrowthVelocity;
		glm::vec3	m_p_maxInitialGrowthVelocity;


		ColorNode m_p_colorsOverLifetime[6];
		int m_colorsUsed;

		bool m_useAdditiveBlending;
		bool m_p_positionFollowsRotation;
		bool m_deactivateFaceCulling;

		bool m_p_rangedSpawnDelay;
		float m_p_spawnDelay;
		float m_p_minSpawnDelay;
		float m_p_maxSpawnDelay;

		bool m_stopSizeAtZeroX;
		bool m_stopSizeAtZeroY;
		bool m_stopSizeAtZeroZ;

		bool m_billboardLockAxisX;
		bool m_billboardLockAxisY;
		bool m_billboardLockAxisZ;

		ColorNode m_newColorsNodes[20] = { ColorNode() }; //--> to fix colors later

		bool m_p_growUniformly;
		float m_p_uniformGrowthVal;
		float m_p_minUniformGrowthVal;
		float m_p_maxUniformGrowthVal;

		bool m_destroyOnFinishActive;
		
		bool m_p_uniformStartSize;
		float m_p_uniformStartSizeVal;
		float m_p_minUniformStartSizeVal;
		float m_p_maxUniformStartSizeVal;

		bool m_stopSizeAtZero;
		bool m_useMultiplyBlending;
		bool m_destroyActiveParticles;
		bool m_permanentParticles;
		bool m_cycleLifeTime;
		bool m_cycleColors;
		float m_spawnTimer;
		bool m_useNewColorNodes;
		int m_newColorsUsed;

		
		// todo					----------------------------------------
		// 
		// 
		// animations
		// fix colors
		// 

	};
}

REFLECTION_BEGIN(Wiwa::ParticleEmitterComponent)

	REFLECT_MEMBER(m_materialId);
	REFLECT_MEMBER(m_materialPath);
	REFLECT_MEMBER(m_mesh);
	REFLECT_MEMBER(m_meshPath);
	REFLECT_MEMBER(m_meshChanged);
	REFLECT_MEMBER(m_materialChanged);
	REFLECT_MEMBER(m_maxParticles);
	REFLECT_MEMBER(m_activeParticles);
	REFLECT_MEMBER(m_loopSpawning);
	REFLECT_MEMBER(m_spawnRate);

	REFLECT_MEMBER(m_rangedSpawnAmount);
	REFLECT_MEMBER(m_spawnAmount);
	REFLECT_MEMBER(m_minSpawnAmount);
	REFLECT_MEMBER(m_maxSpawnAmount);
	REFLECT_MEMBER(m_p_rangedSpawnRate);
	REFLECT_MEMBER(m_p_minSpawnRate);
	REFLECT_MEMBER(m_p_maxSpawnRate);
	REFLECT_MEMBER(m_startActive);
	REFLECT_MEMBER(m_active);
	REFLECT_MEMBER(m_activeTimeChanged);
	REFLECT_MEMBER(m_rangedTimeActive);
	REFLECT_MEMBER(m_activeOverTime);
	REFLECT_MEMBER(m_ActiveTimer);
	REFLECT_MEMBER(m_initialTimeActive);
	REFLECT_MEMBER(m_minInitialTimeActive);
	REFLECT_MEMBER(m_maxInitialTimeActive);
	REFLECT_MEMBER(m_spawnVolume);
	REFLECT_MEMBER(emitterOwner);
	REFLECT_MEMBER(m_p_lifeTime);
	REFLECT_MEMBER(m_p_rangedLifeTime);
	REFLECT_MEMBER(m_p_minLifeTime);
	REFLECT_MEMBER(m_p_maxLifeTime);
	REFLECT_MEMBER(m_billboardActive);
	REFLECT_MEMBER(m_p_followEmitterPosition);
	REFLECT_MEMBER(m_p_followEmitterPositionSpawn);
	REFLECT_MEMBER(m_p_initialPosition);
	REFLECT_MEMBER(m_p_initialPositionSphCenter);
	REFLECT_MEMBER(m_p_initialPositionSphRadius);
	REFLECT_MEMBER(m_p_initialPositionBoxA);
	REFLECT_MEMBER(m_p_initialPositionBoxB);
	REFLECT_MEMBER(m_p_positionTowardsPoint);
	REFLECT_MEMBER(m_p_positionTowardsPointPos);
	REFLECT_MEMBER(m_p_translationTime);
	REFLECT_MEMBER(m_p_initialVelocity);
	REFLECT_MEMBER(m_p_rangedVelocity);
	REFLECT_MEMBER(m_p_minVelocity);
	REFLECT_MEMBER(m_p_maxVelocity);
	REFLECT_MEMBER(m_p_useGravity);
	REFLECT_MEMBER(m_p_gravity);
	REFLECT_MEMBER(m_p_followEmitterRotation);
	REFLECT_MEMBER(m_p_followEmitterRotationSpawn);
	REFLECT_MEMBER(m_p_initialRotation);
	REFLECT_MEMBER(m_p_rangedInitialRotation);
	REFLECT_MEMBER(m_p_minInitialRotation);
	REFLECT_MEMBER(m_p_maxInitialRotation);
	REFLECT_MEMBER(m_p_initialAngularVelocity);
	REFLECT_MEMBER(m_p_rangedAngularVelocity);
	REFLECT_MEMBER(m_p_minInitialAngularVelocity);
	REFLECT_MEMBER(m_p_maxInitialAngularVelocity);
	REFLECT_MEMBER(m_p_rotateTime);
	REFLECT_MEMBER(m_p_rotationOverTime);
	REFLECT_MEMBER(m_p_lifeTime);
	REFLECT_MEMBER(m_p_rotationOverTimePerStart);
	REFLECT_MEMBER(m_p_rotationOverTimePerEnd);
	REFLECT_MEMBER(m_p_rotationOverTimeStart);
	REFLECT_MEMBER(m_p_rotationOverTimeEnd);
	REFLECT_MEMBER(m_p_scaleOverTimeStart);
	REFLECT_MEMBER(m_p_scaleOverTimeEnd);
	REFLECT_MEMBER(m_p_initialGrowthVelocity);
	REFLECT_MEMBER(m_p_rangedGrowthVelocity);
	REFLECT_MEMBER(m_p_minInitialGrowthVelocity);
	REFLECT_MEMBER(m_p_maxInitialGrowthVelocity);
	REFLECT_MEMBER(m_p_colorsOverLifetime);
	REFLECT_MEMBER(m_colorsUsed);
	REFLECT_MEMBER(m_useAdditiveBlending);
	REFLECT_MEMBER(m_p_positionFollowsRotation);
	REFLECT_MEMBER(m_deactivateFaceCulling);
	REFLECT_MEMBER(m_p_rangedSpawnDelay);
	REFLECT_MEMBER(m_p_spawnDelay);
	REFLECT_MEMBER(m_p_minSpawnDelay);
	REFLECT_MEMBER(m_p_maxSpawnDelay);
	REFLECT_MEMBER(m_stopSizeAtZeroX);
	REFLECT_MEMBER(m_stopSizeAtZeroY);
	REFLECT_MEMBER(m_stopSizeAtZeroZ);
	REFLECT_MEMBER(m_billboardLockAxisX);
	REFLECT_MEMBER(m_billboardLockAxisY);
	REFLECT_MEMBER(m_billboardLockAxisZ);
	REFLECT_MEMBER(m_newColorsNodes);
	REFLECT_MEMBER(m_p_growUniformly);
	REFLECT_MEMBER(m_p_uniformGrowthVal);
	REFLECT_MEMBER(m_p_minUniformGrowthVal);
	REFLECT_MEMBER(m_p_maxUniformGrowthVal);
	REFLECT_MEMBER(m_destroyOnFinishActive);
	REFLECT_MEMBER(m_permanentParticles);
	REFLECT_MEMBER(m_cycleLifeTime);
	REFLECT_MEMBER(m_cycleColors);
	REFLECT_MEMBER(m_spawnTimer);
	REFLECT_MEMBER(m_useNewColorNodes);
	REFLECT_MEMBER(m_newColorsUsed);

REFLECTION_END;