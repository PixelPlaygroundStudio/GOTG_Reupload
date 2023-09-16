#include <wipch.h>
#include "ItemManager.h"

#include <Wiwa/utilities/json/JSONDocument.h>

std::map<std::string, Wiwa::Ability> Wiwa::ItemManager::m_AbilityPool;
std::map<std::string, Wiwa::PassiveSkill> Wiwa::ItemManager::m_PassiveSkillPool;
std::map<std::string, Wiwa::Buff> Wiwa::ItemManager::m_BuffPool;
std::map<std::string, Wiwa::Consumable> Wiwa::ItemManager::m_ConsumablePool;
std::map<std::string, Wiwa::ShopElement> Wiwa::ItemManager::m_ShopElementPool;

Wiwa::ItemManager::~ItemManager()
{
	m_AbilityPool.clear();
	m_PassiveSkillPool.clear();
	m_BuffPool.clear();
	m_ConsumablePool.clear();
	m_ShopElementPool.clear();
}

void Wiwa::ItemManager::AddAbility(const Ability& ability)
{
	m_AbilityPool.emplace(ability.Name, ability);
}

void Wiwa::ItemManager::DeleteAbility(const std::string& name)
{
	m_AbilityPool.erase(name);
}

void Wiwa::ItemManager::AddPassive(const PassiveSkill& passive)
{
	m_PassiveSkillPool[passive.Name] = passive;
}

void Wiwa::ItemManager::DeletePassive(const std::string& name)
{
	m_PassiveSkillPool.erase(name);
}

void Wiwa::ItemManager::AddBuff(const Buff& buff)
{
	m_BuffPool[buff.Name] = buff;
}

void Wiwa::ItemManager::DeleteBuff(const std::string& name)
{
	m_BuffPool.erase(name);
}

void Wiwa::ItemManager::AddConsumable(const Consumable& consumable)
{
	m_ConsumablePool[consumable.Name] = consumable;
}

void Wiwa::ItemManager::DeleteConsumable(const std::string& name)
{
	m_ConsumablePool.erase(name);
}

void Wiwa::ItemManager::AddShopElement(const ShopElement& shopElement)
{
	m_ShopElementPool[shopElement.Name] = shopElement;
}

void Wiwa::ItemManager::AugmentShopElement(const std::string& name)
{
	m_ShopElementPool[name].AugmentStep();
}

void Wiwa::ItemManager::DeleteShopElement(const std::string& name)
{
	m_ShopElementPool.erase(name);
}

void Wiwa::ItemManager::UnlockShopElement(const std::string& name ,bool ret)
{
	m_ShopElementPool[name].Unlocked = ret;
}

void Wiwa::ItemManager::Serialize(JSONDocument* doc)
{
	JSONValue actives = doc->AddMemberArray("actives");
	for (const auto& active : m_AbilityPool)
	{
		JSONValue activeObj = actives.PushBackObject();
		activeObj.AddMember("name", active.second.Name.c_str());
		activeObj.AddMember("description", active.second.Description.c_str());
		activeObj.AddMember("path", Resources::getResourcePathById<Image>(active.second.Icon));
		activeObj.AddMember("damage", active.second.Damage);
		activeObj.AddMember("range", active.second.Range);
		activeObj.AddMember("area", active.second.Area);
		activeObj.AddMember("cooldown", active.second.Cooldown);
		activeObj.AddMember("price", active.second.Price);
		activeObj.AddMember("type", (int)active.second.AbilityType);
		activeObj.AddMember("tag_1", (int)active.second.itemTag[0]);
		activeObj.AddMember("tag_2", (int)active.second.itemTag[1]);

	}

	JSONValue passives = doc->AddMemberArray("passives");
	for (const auto& passive : m_PassiveSkillPool)
	{
		JSONValue passiveObj = passives.PushBackObject();
		passiveObj.AddMember("name", passive.second.Name.c_str());
		passiveObj.AddMember("description", passive.second.Description.c_str());
		passiveObj.AddMember("path", Resources::getResourcePathById<Image>(passive.second.Icon));
		passiveObj.AddMember("type", (int)passive.second.PassiveType);
	}

	JSONValue buffs = doc->AddMemberArray("buffs");
	for (const auto& buff : m_BuffPool)
	{
		JSONValue buffObj = buffs.PushBackObject();
		buffObj.AddMember("name", buff.second.Name.c_str());
		buffObj.AddMember("description", buff.second.Description.c_str());
		buffObj.AddMember("path", Resources::getResourcePathById<Image>(buff.second.Icon));
		buffObj.AddMember("buff_percent", buff.second.BuffPercent);
		buffObj.AddMember("duration", buff.second.Duration);
		buffObj.AddMember("cooldown", buff.second.Cooldown);
		buffObj.AddMember("price", buff.second.Price);
		buffObj.AddMember("type", (int)buff.second.buffType);
	}

	JSONValue consumables = doc->AddMemberArray("consumables");
	for (const auto& consumable : m_ConsumablePool)
	{
		JSONValue consumObj = consumables.PushBackObject();
		consumObj.AddMember("name", consumable.second.Name.c_str());
		consumObj.AddMember("path", Resources::getResourcePathById<Image>(consumable.second.Icon));
		consumObj.AddMember("type", (int)consumable.second.Type);
		consumObj.AddMember("buff_percent", consumable.second.BuffPercent);
		consumObj.AddMember("is_egos_help", consumable.second.IsEgosHelp);
	}

	JSONValue shopElements = doc->AddMemberArray("shop_elements");
	for (const auto& shopelement : m_ShopElementPool)
	{
		JSONValue shop_element = shopElements.PushBackObject();
		shop_element.AddMember("name", shopelement.second.Name.c_str());
		shop_element.AddMember("steps", shopelement.second.AmountOfSteps);
		shop_element.AddMember("current_step", shopelement.second.CurrentStep);
		JSONValue shopCosts = shop_element.AddMemberArray("costs").PushBackObject();
		for (int i = 0; i < shopelement.second.AmountOfSteps; i++)
		{
			char buffer[100];
			std::sprintf(buffer, "%s%d", "costs_", i);
			shopCosts.AddMember(buffer, shopelement.second.Costs.at(i));
		}
		JSONValue shopPercents = shop_element.AddMemberArray("percentages").PushBackObject();
		for (int i = 0; i < shopelement.second.AmountOfSteps; i++)
		{
			char buffer[100];
			std::sprintf(buffer, "%s%d", "percentages_", i);
			shopPercents.AddMember(buffer, shopelement.second.PercentageIncreases.at(i));
		}
		shop_element.AddMember("howard_passive_type", (int)shopelement.second.PassiveBoost);
		shop_element.AddMember("unlocking_method", (int)shopelement.second.unlockingMethod);
		shop_element.AddMember("amountToUnlock", shopelement.second.amountForUnlocking);
		shop_element.AddMember("countForUnlocking", shopelement.second.countForUnlocking);

		shop_element.AddMember("unlocked", (bool)shopelement.second.Unlocked);
	}
}

void Wiwa::ItemManager::Deserialize(JSONDocument* doc)
{
	JSONDocument& document = *doc;
	if (doc->HasMember("actives"))
	{
		JSONValue actives = document["actives"];
		if (actives.IsArray())
		{
			for (uint32_t i = 0; i < actives.Size(); i++)
			{
				Wiwa::Ability ability;
				ability.Name = actives[i]["name"].as_string();
				ability.Description = actives[i]["description"].as_string();
				ability.Icon = Resources::Load<Image>(actives[i]["path"].as_string());
				ability.Damage = actives[i]["damage"].as_int();
				ability.Range = actives[i]["range"].as_float();
				ability.Area = actives[i]["area"].as_float();
				ability.Cooldown = actives[i]["cooldown"].as_float();
				ability.Price = actives[i]["price"].as_int();
				ability.AbilityType = (AbilityType)actives[i]["type"].as_int();
				ability.itemTag[0] = (ItemTags)actives[i]["tag_1"].as_int();
				ability.itemTag[1] = (ItemTags)actives[i]["tag_2"].as_int();
				AddAbility(ability);
			}
		}
	}
	if (doc->HasMember("passives"))
	{
		JSONValue passives = document["passives"];
		if (passives.IsArray())
		{
			for (uint32_t i = 0; i < passives.Size(); i++)
			{
				Wiwa::PassiveSkill ability;
				ability.Name = passives[i]["name"].as_string();
				ability.Description = passives[i]["description"].as_string();
				ability.Icon = Resources::Load<Image>(passives[i]["path"].as_string());
				ability.PassiveType = (PassiveType)passives[i]["type"].as_int();
				AddPassive(ability);
			}
		}
	}
	if (doc->HasMember("buffs"))
	{
		JSONValue actives = document["buffs"];
		if (actives.IsArray())
		{
			for (uint32_t i = 0; i < actives.Size(); i++)
			{
				Wiwa::Buff ability;
				ability.Name = actives[i]["name"].as_string();
				ability.Description = actives[i]["description"].as_string();
				ability.Icon = Resources::Load<Image>(actives[i]["path"].as_string());
				ability.BuffPercent = actives[i]["buff_percent"].as_int();
				ability.Duration = actives[i]["duration"].as_float();
				ability.Cooldown = actives[i]["cooldown"].as_float();
				ability.Price = actives[i]["price"].as_int();
				ability.buffType = (BuffType)actives[i]["type"].as_int();
				AddBuff(ability);
			}
		}
	}
	if (doc->HasMember("consumables"))
	{
		JSONValue actives = document["consumables"];
		if (actives.IsArray())
		{
			for (uint32_t i = 0; i < actives.Size(); i++)
			{
				Wiwa::Consumable ability;
				ability.Name = actives[i]["name"].as_string();
				ability.Type = (ConsumableType)actives[i]["type"].as_int();
				ability.Icon = Resources::Load<Image>(actives[i]["path"].as_string());
				ability.BuffPercent = actives[i]["buff_percent"].as_int();
				ability.IsEgosHelp = actives[i]["is_egos_help"].as_bool();
				AddConsumable(ability);
			}
		}
	}
	if (doc->HasMember("shop_elements"))
	{
		if (JSONValue shop_elements = (*doc)["shop_elements"]; shop_elements.IsArray())
		{
			for (uint32_t i = 0; i < shop_elements.Size(); i++)
			{
				Wiwa::ShopElement shopElement;
				if (JSONValue shop_elements = (*doc)["shop_elements"]; shop_elements.IsArray())
				{
					Wiwa::ShopElement shopElement;
					shopElement.Name = shop_elements[i]["name"].as_string();
					shopElement.AmountOfSteps = shop_elements[i]["steps"].as_int();
					shopElement.CurrentStep = shop_elements[i]["current_step"].as_int();
					if (JSONValue shop_element_costs = shop_elements[i]["costs"]; shop_element_costs.IsArray())
					{
						for (uint32_t j = 0; j < shop_element_costs.Size(); j++)
						{
							for (uint32_t k = 0; k < 5; k++)
							{
								char buffer[100];
								std::sprintf(buffer, "%s%d", "costs_", k);
								if (shop_element_costs[j].HasMember(buffer))
									shopElement.Costs.push_back(shop_element_costs[j][buffer].as_int());
							}
						}
					}
					if(JSONValue shop_element_percentages = shop_elements[i]["percentages"]; shop_element_percentages.IsArray())
					{
						for (uint32_t j = 0; j < shop_element_percentages.Size(); j++)
						{
							for (uint32_t k = 0; k < 5; k++)
							{
								char buffer[100];
								std::sprintf(buffer, "%s%d", "percentages_", k);
								if (shop_element_percentages[j].HasMember(buffer))
									shopElement.PercentageIncreases.push_back(shop_element_percentages[j][buffer].as_int());
							}
						}
					}
					shopElement.PassiveBoost = (HowardElementType)shop_elements[i]["howard_passive_type"].as_int();
					shopElement.unlockingMethod = (ShopElementUnlockingMethod)shop_elements[i]["unlocking_method"].as_int();
					shopElement.amountForUnlocking = shop_elements[i]["amountToUnlock"].as_int();
					shopElement.countForUnlocking = shop_elements[i]["countForUnlocking"].as_int();
					shopElement.Unlocked = shop_elements[i]["unlocked"].as_bool();

					AddShopElement(shopElement);
				}
			}
		}
	}
}

Wiwa::Ability* Wiwa::ItemManager::GetAbility(const char* name)
{
	return &(*m_AbilityPool.find(name)).second;
}

Wiwa::PassiveSkill* Wiwa::ItemManager::GetPassive(const char* name)
{
	return &(*m_PassiveSkillPool.find(name)).second;
}

Wiwa::Buff* Wiwa::ItemManager::GetBuff(const char* name)
{
	return &(*m_BuffPool.find(name)).second;
}

Wiwa::Consumable* Wiwa::ItemManager::GetConsumable(const char* name)
{
	return &(*m_ConsumablePool.find(name)).second;
}

Wiwa::ShopElement* Wiwa::ItemManager::GetShopElement(const char* name)
{
	return &(*m_ShopElementPool.find(name)).second;
}

