#pragma once

#include <string>

#include <Wiwa/core/Resources.h>

namespace Wiwa
{
	enum class CooldownState
	{
		NO_CHARGED,
		STARTING_CHARGE,
		MEDIUM_CHARGE,
		FULLY_CHARGED
	};
	enum class ConsumableType
	{
		HEAL,
		SHIELD 
	};
	enum class AbilityType
	{
		YONDUS_FIN,
		GROOTS_SEEDS,
		PHYLAS_QUANTUM_SWORD,
		STARHAWKS_BLAST,
		CAPTAINS_UNIVERSE,
		MANTIS_TELEPATHIC_THRUST
	};
	enum class BuffType
	{
		MAJOR_VICTORY_SHIELD,
		NIKKIS_TOUCH,
		COSMOS_PAW,
		MARTINEX_THERMOKINESIS,
		BUGS_LEGS,
		CHARLIE27_FIST
	};
	// Defines which attributes will be modified via the passive
	enum class PassiveType
	{
		MOVEMENT,
		ROF,
		BUFF,
		ATTACK,
		PROJECTILE,
		HEALTH,
		RANGE,
		SHIELD_CHARGE,
		
	};
	enum class HowardElementType
	{
		HEALTH_CAPACITOR,
		NANO_BOOST,
		EASY_TRIGGER,
		FANCY_BOOTS,
		LETHAL_SHOOTER,
		SHIELD_FAN,
		NANO_MACHINES,
		RECOVERY_SHIELD,
		SECOND_WIND,
		REROLL,
		BEGINNERS_LUCK,
		MIDAS_TOUCH,
		DEVOURER,
		FANATIC,
		RECOVERY_HEALTH,
		ULTIMATE_MIDAS_TOUCH,
		FRIENDLY_FACE
	};
	enum class ShopElementUnlockingMethod
	{
		ENEMIES_KILLED,
		ITEMS_BOUGHT,
		ULTRON_KILLED
	};
	enum class ItemTags
	{
		ATTACK,
		PROJECTILE,
		AOE,
		DEBUFF,
		HOMING,
	};
	struct Ability
	{
		std::string Name;
		std::string Description;
		
		ResourceId Icon;

		int Damage;
		float Range;
		float Area;
		float Cooldown;
		float CurrentTime;
		int Price;

		AbilityType AbilityType;

		CooldownState CooldownState;

		ItemTags itemTag[2];
		Ability() = default;
		Ability(const Ability& ability)
		{
			this->Name = ability.Name;
			this->Description = ability.Description;
			this->Icon = ability.Icon;
			this->Damage = ability.Damage;
			this->Range = ability.Range;
			this->Area = ability.Area;
			this->Cooldown = ability.Cooldown;
			this->CurrentTime = ability.CurrentTime;
			this->Price = ability.Price;
			this->AbilityType = ability.AbilityType;
			this->CooldownState = ability.CooldownState;
			this->itemTag[0] = ability.itemTag[0];
			this->itemTag[1] = ability.itemTag[1];
		}
		
		Ability(const char* name)
			: Name(name),
			Description(""),
			Icon(0),
			Damage(0),
			Range(0.f),
			Area(0.f),
			Cooldown(0.f),
			CurrentTime(0.f),
			Price(0),
			AbilityType(AbilityType::YONDUS_FIN),
			CooldownState(CooldownState::NO_CHARGED),
			itemTag{ItemTags::AOE,ItemTags::ATTACK}

		{}

		void Use();
		void YondusFin();
		void GrootsSeeds();
		void PhylasQuantumSword();
		void StarhawksBlast();
		void CaptainsUniverse();
		void MantisTelepathicThrust();
	};


	struct PassiveSkill
	{
		std::string Name;
		std::string Description;

		ResourceId Icon;

		int BuffPercent;
		
		PassiveType PassiveType;

		PassiveSkill() = default;
		PassiveSkill(const PassiveSkill& passive)
		{
			this->Name = passive.Name;
			this->Description = passive.Description;
			this->BuffPercent = passive.BuffPercent;
			this->Icon = passive.Icon;
			this->PassiveType = passive.PassiveType;
		}
		PassiveSkill(const char* name)
			: Name(name),
		      Description(""),
		      Icon(0),
			  BuffPercent(0),
			  PassiveType(PassiveType::ATTACK)
		{}

		void Use();
		void DisplayStats();
	};

	struct Buff
	{
		std::string Name;
		std::string Description;

		ResourceId Icon;

		BuffType buffType;

		int BuffPercent;
		float Duration;
		float Cooldown;
		float CurrentTime;
		float CoolDownTimer;
		int Price;
		bool IsActive;
		int MaxHealthInc;
		int HealthInc;
		int MaxShieldInc;
		int ShieldInc;
		float RoFInc;
		int AttackDmgInc;
		float MovSpeedInc;

		CooldownState CooldownState;

		Buff()
			: Name(""),
			  Description(""),
			  Icon(0),
			  buffType(BuffType::MAJOR_VICTORY_SHIELD),
			  BuffPercent(0),
			  Duration(0.f),
			  Cooldown(0.f),
			  CurrentTime(0.f),
			  CoolDownTimer(0.f),
			  Price(0),
			  IsActive(false),
			  CooldownState(CooldownState::NO_CHARGED)
		{};
		Buff(const Buff& buff)
		{
			this->Name = buff.Name;
			this->Icon = buff.Icon;
			this->buffType = buff.buffType;
			this->BuffPercent = buff.BuffPercent;
			this->Duration = buff.Duration;
			this->Cooldown = buff.Cooldown;
			this->Price = buff.Price;
			this->CurrentTime = buff.CurrentTime;
			this->CoolDownTimer = buff.CoolDownTimer;
			this->CooldownState = buff.CooldownState;
		}
		Buff(const char* name)
			: Name(name),
		      Description(""),
			  Icon(0),
			  buffType(BuffType::MAJOR_VICTORY_SHIELD),
			  BuffPercent(0),
			  Duration(0.f),
			  Cooldown(0.f),
		      CurrentTime(0.f),
              CoolDownTimer(0.f),
			  Price(0),
			  IsActive(false)
		{}

		void Use();
		void UnUse();
	};

	struct Consumable
	{
		std::string Name;
		ResourceId Icon;
		ConsumableType Type;
		int BuffPercent;
		bool IsEgosHelp;
		
		Consumable() = default;
		Consumable(const Consumable& consumable)
		{
			this->Name = consumable.Name;
			this->Icon = consumable.Icon;
			this->Type = consumable.Type;
			this->BuffPercent = consumable.BuffPercent;
			this->IsEgosHelp = consumable.IsEgosHelp;
		}
		Consumable(const char* name)
			: Name(name),
			  Icon(0),
			  Type(ConsumableType::HEAL),
			  BuffPercent(0),
		      IsEgosHelp(false)
		{}
		
		void Use();
	};

	struct ShopElement
	{
		std::string Name;
		int AmountOfSteps;
		int CurrentStep;
		std::vector<int> Costs;
		std::vector<int> PercentageIncreases;
		HowardElementType PassiveBoost;
		bool Unlocked;
		ShopElementUnlockingMethod unlockingMethod;
		int amountForUnlocking;
		int countForUnlocking;
		ShopElement() = default;
		ShopElement(const ShopElement& shopElement)
		{
			this->Name = shopElement.Name;
			this->AmountOfSteps = shopElement.AmountOfSteps;
			this->CurrentStep = shopElement.CurrentStep;
			this->Costs = shopElement.Costs;
			this->PercentageIncreases = shopElement.PercentageIncreases;
			this->PassiveBoost = shopElement.PassiveBoost;
			this->unlockingMethod = shopElement.unlockingMethod;
			this->Unlocked = shopElement.Unlocked;
			this->amountForUnlocking = shopElement.amountForUnlocking;
			this->countForUnlocking = shopElement.countForUnlocking;

		}
		ShopElement(const char* name)
			: Name(name),
			AmountOfSteps(3),
			CurrentStep(0),
			Costs(),
			PercentageIncreases(),
			PassiveBoost(HowardElementType::FANCY_BOOTS),
			unlockingMethod(ShopElementUnlockingMethod::ITEMS_BOUGHT),
			amountForUnlocking(100),
			countForUnlocking(0),
			Unlocked(true)
		{}

		void Use();
		//UnUse, when an item gets upgraded in your inventory we can reduce the percentage increase to re increase it again
		void UnUse();

		void AugmentStep();

		int HealthInc;
		int MaxHealthInc;
		int DamageInc;
		int RofInc;
		float MovSpeedInc;
		int RangeInc;
		int ShieldRegenInc;

	};
}