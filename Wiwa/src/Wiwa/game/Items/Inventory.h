#pragma once

#include <Wiwa/core/Core.h>
#include "Item.h"

#include <vector>

namespace Wiwa
{
	class JSONDocument;
	class WI_API Inventory
	{
	protected:
		Inventory();
		~Inventory();
	public:
		void Serialize(JSONDocument* doc);
		void Deserialize(JSONDocument* doc);

		void InitGame();
		
		void AddAbility(const Ability* ability) const;
		void AddBuff(const Buff* buff) const;
		void AddShopPassive(const ShopElement& shopElement);
		void AddPassive(const PassiveSkill& skill);
		void PushBackPassive(const PassiveSkill& skill);
		void UseAllPassives();
		void AddConsumable(const Consumable& consumable);

		void Update();
		
		void UseAbility(size_t index) const;
		void UseBuff(size_t index) const ;
		void UseShopPassive(size_t index);
		bool CheckIfShopPassiveAlreadyImplemented(std::string name);
		size_t GetShopPassiveIndex(const ShopElement& shopElement);
		void SwapUITexture(ResourceId id, int indexUI);
		void Clear();

		WI_HARD_INL void AddTokens(uint32_t tokens) { m_Tokens += tokens; }
		WI_HARD_INL void SubstractTokens(uint32_t tokens) { m_Tokens -= tokens; }

		WI_HARD_INL void AddTokensHoward(uint32_t tokens) { m_Tokens_Howard += tokens; }
		WI_HARD_INL void SubstractTokensHoward(uint32_t tokens) { m_Tokens_Howard -= tokens; }

		WI_HARD_INL Ability**& GetAbilities() { return m_Abilities; }
		WI_HARD_INL Buff**& GetBuffs() { return m_Buffs; }
		WI_HARD_INL std::vector<ShopElement>& GetShopPassives() { return m_ShopPassives; }
		WI_HARD_INL std::vector<PassiveSkill>& GetPassives() { return m_PassiveSkill; }
		WI_HARD_INL uint32_t GetTokens() const { return m_Tokens; }
		WI_HARD_INL uint32_t GetTokensHoward() const { return m_Tokens_Howard; }
		
	private:
		Ability** m_Abilities;
		Buff** m_Buffs;
		Consumable m_Consumable;
		std::vector<ShopElement> m_ShopPassives;

		std::vector<PassiveSkill> m_PassiveSkill;
		uint32_t m_Tokens;
		uint32_t m_Tokens_Howard;
		friend class GameStateManager;
	};
}