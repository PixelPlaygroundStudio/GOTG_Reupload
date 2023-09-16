#include "GameLogPanel.h"
#include <Wiwa/game/GameStateManager.h>
#include "../../Utils/EditorUtils.h"
#include <Wiwa/scene/Scene.h>
#include <Wiwa/utilities/time/Time.h>

#include "Wiwa/ecs/components/game/Character.h"

GameLogPanel::GameLogPanel(EditorLayer* instance)
	: Panel("Game Log", ICON_FK_GAMEPAD, instance)
{
}

GameLogPanel::~GameLogPanel()
{
}

void GameLogPanel::Draw()
{
	ImGui::Begin(iconName.c_str(), &active);
	ImGui::TextColored(ImVec4(102, 0, 255, 1), "Panel to check information about room");
	ImGui::Text("Save file found %i", Wiwa::GameStateManager::s_CanContinue);
	ImGui::Checkbox("God mode", &Wiwa::GameStateManager::s_GodMode);

	if (ImGui::CollapsingHeader("Chances"))
	{

		ImGui::InputInt("Enemy item drop chance", &Wiwa::GameStateManager::s_EnemyDropChances);
		ImGui::Separator();
		ImGui::InputInt("Active skill drop chance", &Wiwa::GameStateManager::s_ActiveSkillChances);
		ImGui::InputInt("Passive skill drop chance", &Wiwa::GameStateManager::s_PassiveSkillChances);
		ImGui::InputInt("Buff drop chance", &Wiwa::GameStateManager::s_BuffChances);
		ImGui::InputInt("NPC room chance", &Wiwa::GameStateManager::s_NPCRoomChances);
	}

	DrawStateInfo();
	DrawRoomSpawnersInfo();
	DrawRoomVariables();
	ImGui::End();
}

void GameLogPanel::DrawStateInfo()
{
	const char* roomType = Wiwa::GameStateManager().GetRoomType();
	ImGui::Text("Room type: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 255, 200, 1), roomType);

	const char* roomState = Wiwa::GameStateManager().GetRoomState();
	ImGui::Text("Room state: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 255, 200, 1), roomState);
}

void GameLogPanel::DrawRoomSpawnersInfo()
{
	ImGui::Text("Has finished: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 0, 200, 1), "%s", Wiwa::GameStateManager::s_HasFinshedRoom ? "true" : "false");

	ImGui::Text("Is triggering next: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 0, 200, 1), "%s", Wiwa::GameStateManager::s_PlayerTriggerNext ? "true" : "false");

	
	if (Wiwa::Time::IsPlaying())
	{
		ImGui::Text("Enemies active: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(255, 0, 0, 1), "%d", Wiwa::GameStateManager::GetActiveEnemies());

		ImGui::Text("Approximate total enemies: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(255, 0, 0, 1), "%d", Wiwa::GameStateManager::GetAproximateTotalEnemies());
	}

	ImGui::Separator();

	ImGui::Text("Current difficulty level: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(255, 0, 0, 1), "%d", Wiwa::GameStateManager::GetEnemyManager().m_CurrentRunLevel);

	ImGui::Text("Count of combat rooms: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(255, 0, 0, 1), "%d", Wiwa::GameStateManager::GetEnemyManager().m_CurrentCombatRoomsCount);

	ImGui::Text("Count of reward rooms: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(255, 0, 0, 1), "%d", Wiwa::GameStateManager::GetEnemyManager().m_CurrentRewardRoomCount);
}

void GameLogPanel::DrawRoomVariables()
{
	ImGui::Separator();
	ImGui::Text("Rooms to boss %d", Wiwa::GameStateManager::s_RoomsToBoss);
	ImGui::Text("Rooms to shop %d", Wiwa::GameStateManager::s_RoomsToShop);
	ImGui::Text("Rooms to end and return tu hub %d", Wiwa::GameStateManager::s_CurrentRoomsCount);
	ImGui::InputInt("HUB room index", (int*)&Wiwa::GameStateManager::s_HUBRoomIndx);
	ImGui::InputInt("Boss room index", (int*)&Wiwa::GameStateManager::s_BossRoomIndx);
	ImGui::InputInt("Rooms to boss", (int*)&Wiwa::GameStateManager::s_RoomsToBoss);
	ImGui::InputInt("Rooms to shop", (int*)&Wiwa::GameStateManager::s_RoomsToShop);
	ImGui::InputInt("Next reward", &Wiwa::GameStateManager::s_NextRewardRoomReward);
	ImGui::InputInt("Left door reward", &Wiwa::GameStateManager::s_DoorsReward[0]);
	ImGui::InputInt("Right door reward", &Wiwa::GameStateManager::s_DoorsReward[1]);
	ImGui::Checkbox("Set phalanx rooms", &Wiwa::GameStateManager::s_CanPhalanxRooms);

	ImGui::Separator();
	ImGui::PushID(0);
	ImGui::Text("Battle rooms");
	ImGui::Text("Kree rooms");
	VectorEdit(Wiwa::GameStateManager::s_CombatRooms);
	ImGui::PopID();
	ImGui::PushID(420);
	ImGui::Text("Phalanx rooms");
	VectorEdit(Wiwa::GameStateManager::s_PhalanxRooms);
	ImGui::PopID();
	ImGui::Separator();
	ImGui::PushID(1);
	ImGui::Text("Reward rooms");
	VectorEdit(Wiwa::GameStateManager::s_RewardRooms);
	ImGui::Separator();
	ImGui::PopID();
	ImGui::PushID(2);
	ImGui::Text("Shop rooms");
	VectorEdit(Wiwa::GameStateManager::s_ShopRooms);
	ImGui::PopID();
	ImGui::Separator();

	ImGui::PushID(3);
	ImGui::Text("Watcher rooms");
	VectorEdit(Wiwa::GameStateManager::s_WatcherRooms);
	ImGui::PopID();
	ImGui::Separator();

	ImGui::Text("Current player id %i", Wiwa::GameStateManager::s_PlayerId);

	const char* startCharacter
		= Wiwa::GameStateManager::s_CurrentCharacter == 0 ? "StarLord" : Wiwa::GameStateManager::s_CurrentCharacter == 1 ? "Rocket" : "None";
	ImGui::Text("Current character %s", startCharacter);
	ImGui::InputInt("Start character", (int*)&Wiwa::GameStateManager::s_CurrentCharacter);
	CLAMP(Wiwa::GameStateManager::s_CurrentCharacter, 0, 1);

	if (ImGui::CollapsingHeader("Current player stats"))
	{
		if (Wiwa::GameStateManager::s_CurrentScene && Wiwa::Time::IsPlaying())
		{
			if (Wiwa::GameStateManager::s_CharacterStats.c_id != WI_INVALID_INDEX)
			{
				Wiwa::Character* character =
					(Wiwa::Character*)Wiwa::GameStateManager::s_CurrentScene->GetEntityManager().GetComponentByIterator(Wiwa::GameStateManager::s_CharacterStats);
				if (character)
				{
					ImGui::Text("Max Health %i", character->MaxHealth);
					ImGui::Text("Health %i", character->Health);
					ImGui::Text("Max Shield %i", character->MaxShield);
					ImGui::Text("Shield %i", character->Shield);
					ImGui::Text("Shield Regeneration %i", character->ShieldRegeneration);
					ImGui::Text("Shield Regeneration Mult %i", character->ShieldRegenerationMult);
					ImGui::Text("Shield %i", character->Shield);
					ImGui::Text("Damage %i", character->Damage);
					ImGui::Text("Rate of fire %f", character->RateOfFire);
					ImGui::Text("Speed %f", character->Speed);
					ImGui::Text("Dash distance %f", character->DashDistance);
					ImGui::Text("Dash speed %f", character->DashSpeed);
					ImGui::Text("Dash cooldown %f", character->DashCooldown);
					ImGui::Text("Walk threshold %f", character->WalkTreshold);
				}
			}
		}
		else
		{
			ImGui::Text("Not playing");
		}
	}

	if(ImGui::CollapsingHeader("Star Lord default values"))
	{
		ImGui::PushID(0);
		ImGui::InputInt("Max health", &Wiwa::GameStateManager::s_CharacterSettings[0].MaxHealth);
		ImGui::InputInt("Max Shield", &Wiwa::GameStateManager::s_CharacterSettings[0].MaxShield);
		ImGui::InputInt("Damage", &Wiwa::GameStateManager::s_CharacterSettings[0].Damage);
		ImGui::InputFloat("Rate of fire", &Wiwa::GameStateManager::s_CharacterSettings[0].RateOfFire);
		ImGui::InputFloat("Speed", &Wiwa::GameStateManager::s_CharacterSettings[0].Speed);
		ImGui::InputFloat("Dash distance", &Wiwa::GameStateManager::s_CharacterSettings[0].DashDistance);
		ImGui::InputFloat("Dash speed", &Wiwa::GameStateManager::s_CharacterSettings[0].DashSpeed);
		ImGui::InputFloat("Dash cooldown", &Wiwa::GameStateManager::s_CharacterSettings[0].DashCoolDown);
		ImGui::InputFloat("Walk threshold", &Wiwa::GameStateManager::s_CharacterSettings[0].WalkTreshold);

		ImGui::InputInt("shield regeneration", &Wiwa::GameStateManager::s_CharacterSettings[0].ShieldRegeneration);
		ImGui::InputInt("shield regeneration mult", &Wiwa::GameStateManager::s_CharacterSettings[0].ShieldRegenerationMult);

		ImGui::PopID();
	}

	if (ImGui::CollapsingHeader("Rocket default values"))
	{
		ImGui::PushID(1);
		ImGui::InputInt("Max health", &Wiwa::GameStateManager::s_CharacterSettings[1].MaxHealth);
		ImGui::InputInt("Max Shield", &Wiwa::GameStateManager::s_CharacterSettings[1].MaxShield);
		ImGui::InputInt("Damage", &Wiwa::GameStateManager::s_CharacterSettings[1].Damage);
		ImGui::InputFloat("Rate of fire", &Wiwa::GameStateManager::s_CharacterSettings[1].RateOfFire);
		ImGui::InputFloat("Speed", &Wiwa::GameStateManager::s_CharacterSettings[1].Speed);
		ImGui::InputFloat("Dash distance", &Wiwa::GameStateManager::s_CharacterSettings[1].DashDistance);
		ImGui::InputFloat("Dash speed", &Wiwa::GameStateManager::s_CharacterSettings[1].DashSpeed);
		ImGui::InputFloat("Dash cooldown", &Wiwa::GameStateManager::s_CharacterSettings[1].DashCoolDown);
		ImGui::InputFloat("Walk threshold", &Wiwa::GameStateManager::s_CharacterSettings[1].WalkTreshold);

		ImGui::InputInt("shield regeneration", &Wiwa::GameStateManager::s_CharacterSettings[1].ShieldRegeneration);
		ImGui::InputInt("shield regeneration mult", &Wiwa::GameStateManager::s_CharacterSettings[1].ShieldRegenerationMult);

		ImGui::PopID();
	}

	ImGui::InputFloat("Gamepad deadzone", &Wiwa::GameStateManager::s_GamepadDeadzone);

	if (ImGui::Button("Save"))
	{
		Wiwa::GameStateManager::SerializeData();
	}
}



