#pragma once
#pragma warning(disable : 4302)
#pragma warning(disable : 4311)
#include <Wiwa/core/Core.h>
#include <Wiwa/utilities/Reflection.h>

namespace Wiwa {
	struct WI_API Character {
		int MaxHealth;
		int HealthMultiplier;
		int Health;

		int MaxShield;
		int ShieldMultiplier;
		int Shield;
		int ShieldRegeneration;
		int ShieldRegenerationMult;

		int Damage;
		int DamageMultiplier;

		float RateOfFire;
		float RateOfFireMultiplier;

		float Speed;
		float SpeedMultiplier;

		float DashDistance;
		float DashSpeed;

		float DashCooldown;

		float WalkTreshold;

		bool Slowed;
		float CounterSlowed;

		bool CanMove;
	
	};
}

REFLECTION_BEGIN(Wiwa::Character)
	REFLECT_MEMBER(Speed)
	REFLECT_MEMBER(MaxHealth)
	REFLECT_MEMBER(Health)
	REFLECT_MEMBER(MaxShield)
	REFLECT_MEMBER(Shield)
	REFLECT_MEMBER(Damage)
	REFLECT_MEMBER(RateOfFire)
	REFLECT_MEMBER(DashDistance)
	REFLECT_MEMBER(DashSpeed)
	REFLECT_MEMBER(DashCooldown)
	REFLECT_MEMBER(WalkTreshold)
	REFLECT_MEMBER(Slowed)
	REFLECT_MEMBER(CounterSlowed)
	REFLECT_MEMBER(CanMove)
REFLECTION_END;