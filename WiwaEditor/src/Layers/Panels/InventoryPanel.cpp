#include "InventoryPanel.h"

#include <ranges>
#include <Wiwa/game/Items/ItemManager.h>
#include <Wiwa/game/Items/Inventory.h>
#include <Wiwa/game/GameStateManager.h>
#include "../../Utils/EditorUtils.h"



InventoryPanel::InventoryPanel(EditorLayer* instance)
	: Panel("Inventory", ICON_FK_DATABASE, instance)
{
}

InventoryPanel::~InventoryPanel() = default;

void InventoryPanel::Draw()
{
	ImGui::Begin(iconName.c_str(), &active);
	if (ImGui::CollapsingHeader("Items pool"))
	{
		int id = 0;
		ImGui::Text("Abilities");
		DrawAbilityPool(id);
		ImGui::Separator();
		ImGui::Text("Passive skill");
		DrawPassivePool(id);
		ImGui::Separator();
		ImGui::Text("Buffs");
		DrawBuffPool(id);
		ImGui::Separator();
		ImGui::Text("Consumables");
		DrawConsumablePool(id);
		ImGui::Separator();
		ImGui::Text("Shop elements");
		DrawShopElementPool(id);
		if (ImGui::Button("Save"))
		{
			Wiwa::GameStateManager::SerializeData();
		}
	}
	ImGui::Separator();
	if (ImGui::CollapsingHeader("Player inventory"))
	{
		Wiwa::Inventory& playerInventory = Wiwa::GameStateManager::GetPlayerInventory();

		ImGui::Text("Tokens %i", playerInventory.GetTokens());
		
		Wiwa::Ability** abilities = playerInventory.GetAbilities();
		
		ImGui::Text("Abilities");
		ImGui::Separator();
		ImGui::Indent();
		
		ImGui::Text("Slot 1");
		ImGui::Indent();
		if (abilities != nullptr)
		{
			if (abilities[0] != nullptr)
			{
				ImGui::Text("%s", abilities[0]->Name.c_str());
				ImGui::Indent();
				ImGui::Text("Current time %f", abilities[0]->CurrentTime);
				ImGui::Text("Cooldown %f", abilities[0]->Cooldown);
				ImGui::Unindent();
			}
			else
				ImGui::Text("Empty slot");
		}
		
		ImGui::Unindent();
		ImGui::Separator();

		ImGui::Text("Slot 2");
		ImGui::Indent();
		if (abilities != nullptr)
		{
			if (abilities[1] != nullptr)
			{
				ImGui::Text("%s", abilities[1]->Name.c_str());
				ImGui::Indent();
				ImGui::Text("Current time %f", abilities[1]->CurrentTime);
				ImGui::Text("Cooldown %f", abilities[1]->Cooldown);
				ImGui::Unindent();
			}
			else
				ImGui::Text("Empty slot");
		}
		ImGui::Unindent();
		ImGui::Unindent();
		
		ImGui::Separator();
		ImGui::Text("Passives");
		ImGui::Separator();
		ImGui::Indent();
		
		std::vector<Wiwa::PassiveSkill>& passives = playerInventory.GetPassives();
		for (const auto& passive : passives)
		{
			ImGui::Text("Name %s", passive.Name.c_str());
		}
		
		ImGui::Unindent();
		ImGui::Separator();

		Wiwa::Buff** buffs = playerInventory.GetBuffs();
		
		ImGui::Text("Buffs");
		ImGui::Separator();
		ImGui::Indent();
		
		ImGui::Text("Slot 1");
		ImGui::Indent();
		if (buffs != nullptr)
		{


			if (buffs[0] != nullptr)
			{
				ImGui::Text("%s", buffs[0]->Name.c_str());
				ImGui::Indent();
				ImGui::Text("Current time %f", buffs[0]->CurrentTime);
				ImGui::Text("Cooldown %f", buffs[0]->Cooldown);
				ImGui::Text("Duration %f", buffs[0]->Duration);
				ImGui::Text("Cooldown timer %f", buffs[0]->CoolDownTimer);
				ImGui::Unindent();
			}
			else
				ImGui::Text("Empty slot");
		}
		ImGui::Unindent();
		ImGui::Separator();
		
		ImGui::Text("Slot 2");
		ImGui::Indent();
		if (buffs != nullptr)
		{
			if (buffs[1])
			{
				ImGui::Text("%s", buffs[1]->Name.c_str());
				ImGui::Indent();
				ImGui::Text("Current time %f", buffs[1]->CurrentTime);
				ImGui::Text("Cooldown %f", buffs[1]->Cooldown);
				ImGui::Text("Duration %f", buffs[1]->Duration);
				ImGui::Text("Cooldown timer %f", buffs[1]->CoolDownTimer);
				ImGui::Unindent();
			}
			else
				ImGui::Text("Empty slot");
		}
		ImGui::Unindent();

		ImGui::Text("Shop Elements");
		ImGui::Separator();
		ImGui::Indent();

		std::vector<Wiwa::ShopElement>& shopElements = playerInventory.GetShopPassives();
		for (const auto& shopElement : shopElements)
		{
			ImGui::Text("Name %s", shopElement.Name.c_str());
		}

		ImGui::Unindent();
		ImGui::Separator();
	}
	ImGui::End();
}

void InventoryPanel::DrawConsumablePool(int& id)
{
	if (ImGui::Button("Add Consumable"))
	{
		ImGui::OpenPopup("Create consumable");
	}
	if (ImGui::BeginPopup("Create consumable"))
	{
		static std::string name;
		ImGui::InputText("Name", &name);
		ImGui::SameLine();
		if (ImGui::Button("Create"))
		{
			Wiwa::ItemManager::AddConsumable({ name.c_str() });
			name.clear();
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	const std::map<std::string, Wiwa::Consumable>& consumables = Wiwa::ItemManager::GetConsumables();
	if (!consumables.empty())
	{
		const char* types[] =
		{
			"Heal",
			"Shield"
		};

		if (ImGui::BeginTable("consumables", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
		{
			ImGui::TableSetupColumn("Name");
			ImGui::TableSetupColumn("Icon");
			ImGui::TableSetupColumn("Type");
			ImGui::TableSetupColumn("Buff %");
			ImGui::TableHeadersRow();

			for (const auto& key : consumables | std::views::keys)
			{
				Wiwa::Consumable* consumable = Wiwa::ItemManager::GetConsumable(key.c_str());

				ImGui::PushID(id++);
				
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text(consumable->Name.c_str());
				
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.f));
				if (ImGui::Button("Delete"))
				{
					Wiwa::ItemManager::DeleteConsumable(key);
				}
				ImGui::PopStyleColor();

				ImGui::TableNextColumn();
				ImGui::Image((ImTextureID)(intptr_t)Wiwa::Resources::GetResourceById<Wiwa::Image>(consumable->Icon)->GetTextureId(), {64, 64});

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::wstring ws(path);
						std::string pathS(ws.begin(), ws.end());
						std::filesystem::path p = pathS;
						
						if (ImageExtensionComp(p))
						{
							consumable->Icon = Wiwa::Resources::Load<Wiwa::Image>(p.string().c_str());
						}
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::TableNextColumn();
				const char* currentItem = types[(int)consumable->Type];
				if (ImGui::BeginCombo("##type", currentItem))
				{
					for (int i = 0; i < 2; i++)
					{
						const bool isSelected = (currentItem == types[i]);
						if (ImGui::Selectable(types[i], isSelected))
						{
							currentItem = types[i];
							consumable->Type = (Wiwa::ConsumableType)(i);
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}
				ImGui::Checkbox("Is Ego's help", &consumable->IsEgosHelp);
				HelpMarker("If checked the item will behave like Ego's help");
				
				ImGui::TableNextColumn();
				ImGui::InputInt("##percent", &consumable->BuffPercent);

				ImGui::PopID();
			}
			ImGui::EndTable();
		}
	}
	
}

void InventoryPanel::DrawBuffPool(int& id)
{
	std::map<std::string, Wiwa::Buff>& buffs = Wiwa::ItemManager::GetBuffs();
	if (ImGui::Button("Add Buff"))
	{
		ImGui::OpenPopup("Create buff");
	}
	if (ImGui::BeginPopup("Create buff"))
	{
		static std::string name;
		ImGui::InputText("Name", &name);
		ImGui::SameLine();
		if (ImGui::Button("Create"))
		{
			Wiwa::ItemManager::AddBuff({ name.c_str() });
			name.clear();
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	if (!buffs.empty())
	{
		const char* types[] =
		{
			"Major Victory Shield",
			"Nikki's Touch",
			"Cosmo's Paw",
			"Martinex Thermokinesis",
			"Bugs legs",
			"Charlie 27 fist"
		};

		if (ImGui::BeginTable("buffs", 8, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
		{
			ImGui::TableSetupColumn("Name");
			ImGui::TableSetupColumn("Description");
			ImGui::TableSetupColumn("Icon");
			ImGui::TableSetupColumn("Buff %");
			ImGui::TableSetupColumn("Duration");
			ImGui::TableSetupColumn("Cooldown");
			ImGui::TableSetupColumn("Price");
			ImGui::TableSetupColumn("Type");
			ImGui::TableHeadersRow();

			for (const auto& key : buffs | std::views::keys)
			{
				Wiwa::Buff* buff = Wiwa::ItemManager::GetBuff(key.c_str());
				ImGui::PushID(id++);
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text(buff->Name.c_str());
				if(ImGui::Button("Add to inventory"))
				{
					Wiwa::GameStateManager::GetPlayerInventory().AddBuff(buff);
				}
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.f));
				if (ImGui::Button("Delete"))
				{
					Wiwa::ItemManager::DeleteBuff(key);
				}
				ImGui::PopStyleColor();

				ImGui::TableNextColumn();
				ImGui::InputTextMultiline("Description", &buff->Description, {200, 100});


				ImGui::TableNextColumn();
				ImGui::Image((ImTextureID)(intptr_t)Wiwa::Resources::GetResourceById<Wiwa::Image>(buff->Icon)->GetTextureId(), { 64, 64 });

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::wstring ws(path);
						std::string pathS(ws.begin(), ws.end());
						std::filesystem::path p = pathS;

						if (ImageExtensionComp(p))
						{
							buff->Icon = Wiwa::Resources::Load<Wiwa::Image>(p.string().c_str());
						}
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::TableNextColumn();
				ImGui::InputInt("##buffpercent", &buff->BuffPercent);

				ImGui::TableNextColumn();
				ImGui::InputFloat("##duration", &buff->Duration);

				ImGui::TableNextColumn();
				ImGui::InputFloat("##cooldown", &buff->Cooldown);

				ImGui::TableNextColumn();
				ImGui::InputInt("##price", &buff->Price);

				ImGui::TableNextColumn();
				const char* currentBuff = types[(int)buff->buffType];
				if (ImGui::BeginCombo("##type", currentBuff))
				{
					for (int i = 0; i < 6; i++)
					{
						const bool isSelected = (currentBuff == types[i]);
						if (ImGui::Selectable(types[i], isSelected))
						{
							currentBuff = types[i];
							buff->buffType = (Wiwa::BuffType)(i);
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}
				
				ImGui::PopID();
			}
			ImGui::EndTable();
		}

	}
}

void InventoryPanel::DrawPassivePool(int& id)
{
	std::map<std::string, Wiwa::PassiveSkill>& passives = Wiwa::ItemManager::GetSkills();
	if (ImGui::Button("Add Passive"))
	{
		ImGui::OpenPopup("Create passive");
	}
	if (ImGui::BeginPopup("Create passive"))
	{
		static std::string name;
		ImGui::InputText("Name", &name);
		ImGui::SameLine();
		if (ImGui::Button("Create"))
		{
			Wiwa::ItemManager::AddPassive({ name.c_str()});
			name.clear();
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
	if (!passives.empty())
	{
		const char* types[] =
		{
			"Movement",
			"Rate of Fire",
			"Buff",
			"Attack",
			"Projectile",
			"Health",
			"Range",
			"Shield charge"
		};
		if (ImGui::BeginTable("passives", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
		{
			ImGui::TableSetupColumn("Name");
			ImGui::TableSetupColumn("Description");
			ImGui::TableSetupColumn("Icon");
			ImGui::TableSetupColumn("Buff percent");
			ImGui::TableSetupColumn("Type");
			ImGui::TableHeadersRow();

			for (const auto& key : passives | std::views::keys)
			{
				Wiwa::PassiveSkill* passive = Wiwa::ItemManager::GetPassive(key.c_str());
				ImGui::PushID(id++);

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text(passive->Name.c_str());
				if(ImGui::Button("Add to inventory"))
				{
					Wiwa::GameStateManager::GetPlayerInventory().AddPassive(*passive);
				}
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.f));
				if (ImGui::Button("Delete"))
				{
					Wiwa::ItemManager::DeletePassive(key);
				}
				ImGui::PopStyleColor();


				ImGui::TableNextColumn();
				ImGui::InputTextMultiline("##description", &passive->Description, {200, 100});

				ImGui::TableNextColumn();
				ImGui::Image((ImTextureID)(intptr_t)Wiwa::Resources::GetResourceById<Wiwa::Image>(passive->Icon)->GetTextureId(), { 64, 64 });

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::wstring ws(path);
						std::string pathS(ws.begin(), ws.end());
						std::filesystem::path p = pathS;

						if (ImageExtensionComp(p))
						{
							passive->Icon = Wiwa::Resources::Load<Wiwa::Image>(p.string().c_str());
						}
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::TableNextColumn();
				ImGui::InputInt("##buffpercent", &passive->BuffPercent);

				ImGui::TableNextColumn();
				const char* currentItem = types[(int)passive->PassiveType];
				if (ImGui::BeginCombo("##type", currentItem))
				{
					for (int i = 0; i < sizeof(types) / sizeof(char*); i++)
					{
						bool isSelected = (currentItem == types[i]);
						if (ImGui::Selectable(types[i], isSelected))
						{
							currentItem = types[i];
							passive->PassiveType = (Wiwa::PassiveType)(i);
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				ImGui::PopID();
			}
			ImGui::EndTable();
		}
	}
	
}

void InventoryPanel::DrawAbilityPool(int& id)
{
	std::map<std::string, Wiwa::Ability>& abilities = Wiwa::ItemManager::GetAbilities();
	if (ImGui::Button("Add Ability"))
	{
		ImGui::OpenPopup("Create ability");
	}
	if (ImGui::BeginPopup("Create ability"))
	{
		static std::string name;
		ImGui::InputText("Name", &name);
		ImGui::SameLine();
		if (ImGui::Button("Create"))
		{
			Wiwa::ItemManager::AddAbility({ name.c_str() });
			name.clear();
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	if (!abilities.empty())
	{
		const char* types[] =
		{
			"Yondu's Fin",
			"Groot's seeds",
			"Phylas Quantum Sword",
			"StarHawks blast",
			"Captain Universe's Energy",
			"Mantis Telepathic Thrust"
		};
		const char* tags[] =
		{
			"Attack",
			"Projectile",
			"Aoe",
			"Debuff",
			"Homing",
		};
		if (ImGui::BeginTable("actives", 10, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
		{
			ImGui::TableSetupColumn("Name");
			ImGui::TableSetupColumn("Description");
			ImGui::TableSetupColumn("Icon");
			ImGui::TableSetupColumn("Type");
			ImGui::TableSetupColumn("Damage");
			ImGui::TableSetupColumn("Range");
			ImGui::TableSetupColumn("Area");
			ImGui::TableSetupColumn("Cooldown");
			ImGui::TableSetupColumn("Price");
			ImGui::TableSetupColumn("Tags");
			ImGui::TableHeadersRow();
			for (auto& it : abilities)
			{
				Wiwa::Ability* ability = Wiwa::ItemManager::GetAbility(it.first.c_str());
				ImGui::PushID(id++);

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text(ability->Name.c_str());

				if(ImGui::Button("Add to inventory"))
				{
					Wiwa::GameStateManager::GetPlayerInventory().AddAbility(ability);
				}
				
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.f));
				if (ImGui::Button("Delete"))
				{
					Wiwa::ItemManager::DeleteAbility(it.first.c_str());
				}
				ImGui::PopStyleColor();

				ImGui::TableNextColumn();
				ImGui::InputTextMultiline("##description", &ability->Description, {200, 100});

				ImGui::TableNextColumn();

				ImGui::Image((ImTextureID)(intptr_t)Wiwa::Resources::GetResourceById<Wiwa::Image>(ability->Icon)->GetTextureId(), { 64, 64 });

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::wstring ws(path);
						std::string pathS(ws.begin(), ws.end());
						std::filesystem::path p = pathS;

						if (ImageExtensionComp(p))
						{
							ability->Icon = Wiwa::Resources::Load<Wiwa::Image>(p.string().c_str());
						}
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::TableNextColumn();
				
				const char* currentItem = types[(int)ability->AbilityType];
				if (ImGui::BeginCombo("##type", currentItem))
				{
					for (int i = 0; i < 6; i++)
					{
						bool isSelected = (currentItem == types[i]);
						if (ImGui::Selectable(types[i], isSelected))
						{
							currentItem = types[i];
							ability->AbilityType = (Wiwa::AbilityType)(i);
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}
				ImGui::TableNextColumn();
				ImGui::InputInt("##damage", &ability->Damage);

				ImGui::TableNextColumn();
				ImGui::InputFloat("##range", &ability->Range);

				ImGui::TableNextColumn();
				ImGui::InputFloat("##area", &ability->Area);

				ImGui::TableNextColumn();
				ImGui::InputFloat("##cooldown", &ability->Cooldown);

				ImGui::TableNextColumn();
				ImGui::InputInt("##price", &ability->Price);

				ImGui::TableNextColumn();
				const char* currentItem_1 = tags[(int)ability->itemTag[0]];
				if (ImGui::BeginCombo("##tag_1", currentItem_1))
				{
					for (int i = 0; i < sizeof(tags) / sizeof(char*); i++)
					{
						bool isSelected = (currentItem_1 == tags[i]);
						if (ImGui::Selectable(tags[i], isSelected))
						{
							currentItem_1 = tags[i];
							ability->itemTag[0] = (Wiwa::ItemTags)(i);
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}
				const char* currentItem_2 = tags[(int)ability->itemTag[1]];
				if (ImGui::BeginCombo("##tag_2", currentItem_2))
				{
					for (int i = 0; i < sizeof(tags) / sizeof(char*); i++)
					{
						bool isSelected = (currentItem_2 == tags[i]);
						if (ImGui::Selectable(tags[i], isSelected))
						{
							currentItem_2 = tags[i];
							ability->itemTag[1] = (Wiwa::ItemTags)(i);
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}
				ImGui::PopID();
			}
			ImGui::EndTable();
		}
	}
	
}

void InventoryPanel::DrawShopElementPool(int& id)
{
	std::map<std::string, Wiwa::ShopElement>& shopElements = Wiwa::ItemManager::GetShopElements();
	if (ImGui::Button("Add shop element"))
	{
		ImGui::OpenPopup("Create shop element");
	}
	if (ImGui::BeginPopup("Create shop element"))
	{
		static std::string name;
		ImGui::InputText("Name", &name);
		ImGui::SameLine();
		if (ImGui::Button("Create"))
		{
			Wiwa::ItemManager::AddShopElement({ name.c_str() });
			name.clear();
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	if (!shopElements.empty())
	{
		const char* types[] =
		{
			"Health capacitor",
			"Nano boost",
			"Easy trigger",
			"Fancy boots",
			"Lethal shooter",
			"Shield fan",
			"Nano machines",
			"Recovery shield",
			"Second Wind",
			"Reroll",
			"Begginer's luck",
			"Midas touch",
			"Devourer",
			"Fanatic",
			"Recovery health",
			"Ultimate midas touch",
			"Friendly face"
		};
		const char* unlockingMethods[] =
		{
			"Enemies killed",
			"Items bought",
			"Ultron killed count"
		};
		if (ImGui::BeginTable("shop_elements", 9, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
		{
			ImGui::TableSetupColumn("Name");
			ImGui::TableSetupColumn("Unlocked");
			ImGui::TableSetupColumn("Steps");
			ImGui::TableSetupColumn("CurrentStep");
			ImGui::TableSetupColumn("Costs");
			ImGui::TableSetupColumn("PercentageIncreases");
			ImGui::TableSetupColumn("PassiveBoost");
			ImGui::TableSetupColumn("UnlockingMethod");
			ImGui::TableSetupColumn("UnlockingAmount");
			ImGui::TableHeadersRow();
			for (auto& it : shopElements)
			{
				Wiwa::ShopElement* shopElement = Wiwa::ItemManager::GetShopElement(it.first.c_str());
				ImGui::PushID(id++);

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text(shopElement->Name.c_str());

				if (ImGui::Button("Add to inventory"))
				{
					Wiwa::GameStateManager::GetPlayerInventory().AddShopPassive(*shopElement);
				}

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.f));
				if (ImGui::Button("Delete"))
				{
					Wiwa::ItemManager::DeleteShopElement(it.first.c_str());
				}
				ImGui::PopStyleColor();

				ImGui::TableNextColumn();
				ImGui::Checkbox("##description", &shopElement->Unlocked);

				ImGui::TableNextColumn();
				ImGui::InputInt("##amount_of_steps", &shopElement->AmountOfSteps);

				ImGui::TableNextColumn();
				ImGui::InputInt("##current_step", &shopElement->CurrentStep);

				if (shopElement->CurrentStep > shopElement->AmountOfSteps)
					shopElement->CurrentStep = shopElement->AmountOfSteps;

				ImGui::TableNextColumn();
				ImGui::PushID("Costs");
				VectorEdit(shopElement->Costs);
				ImGui::PopID();

				ImGui::TableNextColumn();
				ImGui::PushID("PercentageIncreases");
				VectorEdit(shopElement->PercentageIncreases);
				ImGui::PopID();

				ImGui::TableNextColumn();
				const char* passiveHoward = types[(int)shopElement->PassiveBoost];
				if (ImGui::BeginCombo("##passive_type", passiveHoward))
				{
					for (int i = 0; i < sizeof(types) / sizeof(char*); i++)
					{
						bool isSelected = (passiveHoward == types[i]);
						if (ImGui::Selectable(types[i], isSelected))
						{
							passiveHoward = types[i];
							shopElement->PassiveBoost = (Wiwa::HowardElementType)(i);
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				ImGui::TableNextColumn();
				const char* unlockingMethod = unlockingMethods[(int)shopElement->unlockingMethod];
				if (ImGui::BeginCombo("##unlocking_method", unlockingMethod))
				{
					for (int i = 0; i < sizeof(unlockingMethods) / sizeof(char*); i++)
					{
						bool isSelected = (unlockingMethod == unlockingMethods[i]);
						if (ImGui::Selectable(unlockingMethods[i], isSelected))
						{
							unlockingMethod = unlockingMethods[i];
							shopElement->unlockingMethod = (Wiwa::ShopElementUnlockingMethod)(i);
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				ImGui::TableNextColumn();
				ImGui::InputInt("##amount_to_unlock", &shopElement->amountForUnlocking);
				ImGui::PopID();

			}
			ImGui::EndTable();
		}
	}
}
