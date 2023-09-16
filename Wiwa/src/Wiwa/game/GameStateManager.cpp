#include <wipch.h>
#include "GameStateManager.h"
#include "Wiwa/scene/SceneManager.h"
#include "Items/ItemManager.h"
#include <Wiwa/ecs/components/game/Character.h>
#include <Wiwa/ecs/components/game/items/Item.h>
#include <Wiwa/ecs/components/game/wave/WaveSpawner.h>
#include <Wiwa/ecs/systems/MeshRenderer.h>
#include <Wiwa/audio/Audio.h>
#include <Wiwa/ecs/systems/game/wave/WaveSpawnerSystem.h>
#include <Wiwa/game/GameMusicManager.h>
#include <Wiwa/ecs/systems/game/wave/WaveSystem.h>
#include <Wiwa/ecs/systems/ParticleSystem.h>
#include <Wiwa/ecs/systems/PhysicsSystem.h>
#include <Wiwa/core/Renderer3D.h>

namespace Wiwa
{
	bool GameStateManager::s_CanContinue = false;
	int GameStateManager::s_NextRewardRoomReward = 0;

	int GameStateManager::s_DoorsReward[2] = { 0 };

	RoomType GameStateManager::s_RoomType = RoomType::NONE;
	RoomState GameStateManager::s_RoomState = RoomState::NONE;
	
	bool GameStateManager::s_HasFinshedRoom = false;
	bool GameStateManager::s_CanPassNextRoom = false;
	bool GameStateManager::s_PlayerTriggerNext = false;
	bool GameStateManager::s_GameOver = false;

	bool GameStateManager::debug = true;
	
	int GameStateManager::s_RoomsToShop = 5;
	int GameStateManager::s_RoomsToBoss = 10;

	SceneId GameStateManager::s_BossRoomIndx;
	SceneId GameStateManager::s_HUBRoomIndx;
	SceneId GameStateManager::s_CurrentRoomIndx;
	SceneId GameStateManager::s_IntroductionRoom;
	SceneId GameStateManager::s_LastCombatRoom;
	SceneId GameStateManager::s_LastRewardRoom;
	SceneId GameStateManager::s_LastShopRoom;
	
	bool GameStateManager::s_CanPhalanxRooms = false;
	std::vector<int> GameStateManager::s_CombatRooms;
	std::vector<int> GameStateManager::s_PhalanxRooms;
	std::vector<int> GameStateManager::s_RewardRooms;
	std::vector<int> GameStateManager::s_ShopRooms;
	
	int GameStateManager::s_CurrentRoomsCount;
	DefaultCharacterSettings GameStateManager::s_CharacterSettings[2];

	int GameStateManager::s_CurrentCharacter = STARLORD;
	float GameStateManager::s_GamepadDeadzone = 0.f;
	EntityId GameStateManager::s_PlayerId = WI_INVALID_INDEX;
	
	int GameStateManager::s_ActiveSkillChances = 20;
	int GameStateManager::s_BuffChances = 20;
	int GameStateManager::s_PassiveSkillChances = 35;
	int GameStateManager::s_NPCRoomChances = 25;
	int GameStateManager::s_EnemyDropChances = 100;

	EntityManager::ComponentIterator GameStateManager::s_CharacterStats;
	Scene* GameStateManager::s_CurrentScene = nullptr;
	Inventory* GameStateManager::s_PlayerInventory =  new Inventory();
	GameProgression* GameStateManager::s_GameProgression = new GameProgression();
	GamePoolingManager* GameStateManager::s_PoolManager = new GamePoolingManager();
	EnemyManager* GameStateManager::s_EnemyManager = new EnemyManager();

	bool GameStateManager::FanaticEffect = false;
	int GameStateManager::PrometheanGemsToAdd = 0;
	int GameStateManager::DamageDivisor = 1;
	bool GameStateManager::SecondWind = false;
	bool GameStateManager::s_GodMode = false;

	int GameStateManager::s_CurrentWave = 0;

	std::vector<int> GameStateManager::s_WatcherRooms;

	static std::vector<SceneId> activeScenesKree;
	static std::vector<SceneId> deactiveScenesKree;

	static std::vector<SceneId> activeScenesPhalanx;
	static std::vector<SceneId> deactiveScenesPhalanx;

	int LoadRandomCombatRoom(bool isPhalanx)
	{
		if (isPhalanx)
		{
			if (activeScenesPhalanx.empty())
			{
				for (auto& scene : deactiveScenesPhalanx)
				{
					activeScenesPhalanx.emplace_back(scene);
				}
				deactiveScenesPhalanx.clear();
			}
			std::uniform_int_distribution<> dist(0, activeScenesPhalanx.size() - 1);
			int nextRoom = dist(Application::s_Gen);
			SceneId id = activeScenesPhalanx[nextRoom];
			activeScenesPhalanx.erase(activeScenesPhalanx.begin() + nextRoom);
			deactiveScenesPhalanx.push_back(id);
			SceneManager::ChangeSceneByIndex(id);
			return nextRoom;
		}

		if (activeScenesKree.empty())
		{
			for (auto& scene : deactiveScenesKree)
			{
				activeScenesKree.emplace_back(scene);
			}
			deactiveScenesKree.clear();
		}
		std::uniform_int_distribution<> dist(0, activeScenesKree.size() - 1);
		int nextRoom = dist(Application::s_Gen);
		SceneId id = activeScenesKree[nextRoom];
		activeScenesKree.erase(activeScenesKree.begin() + nextRoom);
		deactiveScenesKree.push_back(id);
		SceneManager::ChangeSceneByIndex(id);
		return nextRoom;
	}

	void GameStateManager::ChangeRoomState(RoomState room_state)
	{
		s_RoomState = room_state;
		if(room_state  == RoomState::STATE_AWAITING_NEXT)
			s_PlayerInventory->AddTokensHoward(PrometheanGemsToAdd);
	}

	void GameStateManager::SaveProgression()
	{
		if(debug)
			WI_CORE_INFO("Saving player progression");

		JSONDocument doc;
		EntityManager& em = s_CurrentScene->GetEntityManager();
		Character* character = em.GetComponent<Character>(s_PlayerId);
		if (character)
		{
			doc.AddMember("max_health", character->MaxHealth);
			doc.AddMember("health", character->Health);
			doc.AddMember("max_shield", character->MaxShield);
			doc.AddMember("shield", character->Shield);
			doc.AddMember("shield_regeneration", character->ShieldRegeneration);
			doc.AddMember("shield_regeneration_mult", character->ShieldRegenerationMult);
			doc.AddMember("damage", character->Damage);
			doc.AddMember("rof", character->RateOfFire);
			doc.AddMember("speed", character->Speed);
			doc.AddMember("dash_distance", character->DashDistance);
			doc.AddMember("dash_speed", character->DashSpeed);
			doc.AddMember("dash_cooldown", character->DashCooldown);
			doc.AddMember("walk_threshold", character->WalkTreshold);
		}

		s_PlayerInventory->Serialize(&doc);
		doc.save_file("config/player_run.json");

		JSONDocument doc_progression;
		s_GameProgression->Serialize(&doc_progression);
		doc_progression.save_file("config/player_progression.json");

		GetEnemyManager().Serialize();
	}

	void GameStateManager::LoadProgression()
	{
		if (debug)
			WI_CORE_INFO("Loading player progression");
		EntityManager& em = s_CurrentScene->GetEntityManager();
		Character* character = em.GetComponent<Character>(s_PlayerId);
		JSONDocument doc("config/player_run.json");

		if (!character)
			return;
		if (doc.IsObject())
		{
			if (doc.HasMember("max_health"))
				character->MaxHealth = doc["max_health"].as_int();
			if (doc.HasMember("health"))
				character->Health = doc["health"].as_int();
			if (doc.HasMember("max_shield"))
				character->MaxShield = doc["max_shield"].as_int();
			if (doc.HasMember("shield"))
				character->Shield = doc["shield"].as_int();
			if (doc.HasMember("shield_regeneration"))
				character->ShieldRegeneration = doc["shield_regeneration"].as_int();
			if (doc.HasMember("shield_regeneration_mult"))
				character->ShieldRegeneration = doc["shield_regeneration_mult"].as_int();
			if (doc.HasMember("damage"))
				character->Damage = doc["damage"].as_int();
			if (doc.HasMember("rof"))
				character->RateOfFire = doc["rof"].as_float();
			if (doc.HasMember("speed"))
				character->Speed = doc["speed"].as_float();
			if (doc.HasMember("dash_distance"))
				character->DashDistance = doc["dash_distance"].as_float();
			if (doc.HasMember("dash_speed"))
				character->DashSpeed = doc["dash_speed"].as_float();
			if (doc.HasMember("dash_cooldown"))
				character->DashCooldown = doc["dash_cooldown"].as_float();
			if (doc.HasMember("walk_threshold"))
				character->WalkTreshold = doc["walk_threshold"].as_float();
		}
		s_PlayerInventory->InitGame();
		s_PlayerInventory->Deserialize(&doc);

		JSONDocument doc_progression("config/player_progression.json");
		s_GameProgression->Deserialize(&doc_progression);

		GetEnemyManager().DeSerialize();
	}

	void GameStateManager::UpdateRoomState()
	{
		if (s_PlayerTriggerNext && s_RoomState == RoomState::STATE_AWAITING_NEXT)
		{
			EndCurrentRoom();
			StartNewRoom();
		}

		if (s_RoomType == RoomType::ROOM_COMBAT)
			UpdateCombatRoom();

		if (s_HasFinshedRoom && s_CanPassNextRoom && s_PlayerTriggerNext)
		{
			ChangeRoomState(RoomState::STATE_AWAITING_NEXT);		
		}
	}

	void GameStateManager::UpdateCombatRoom()
	{
		Wiwa::EntityManager& em = Wiwa::SceneManager::getActiveScene()->GetEntityManager();
		size_t size = 0;
		Wiwa::WaveSpawner* enemySpawnerList = nullptr;
		enemySpawnerList = em.GetComponents<WaveSpawner>(&size);

		bool isFinished = false;
		if (enemySpawnerList){
			for (int i = 0; i < size; i++){
				if (em.IsComponentRemoved<WaveSpawner>(i)) {
				}
				else {
					Wiwa::WaveSpawner* spawner = &enemySpawnerList[i];
					if (spawner) {
						isFinished = IsWaveSpawnerFinished(spawner);
					}

					if (Input::IsKeyPressed(Key::F6))
					{
						WI_INFO("End combat room");
						EndCombatRoom(spawner);
					}
				}
			}
			s_HasFinshedRoom = isFinished;
			s_CanPassNextRoom = s_HasFinshedRoom;
		}
		
		GameMusicManager::UpdateCombatIntesity(GetActiveEnemies());

		if (s_HasFinshedRoom)
		{
			ChangeRoomState(RoomState::STATE_FINISHED);
		}
		else
			ChangeRoomState(RoomState::STATE_STARTED);


	}

	void GameStateManager::NewGame()
	{
		JSONDocument doc;

		JSONValue starlord = doc.AddMemberObject("starlord");
		starlord.AddMember("max_health", s_CharacterSettings[0].MaxHealth);
		starlord.AddMember("max_shield", s_CharacterSettings[0].MaxShield);
		starlord.AddMember("damage", s_CharacterSettings[0].Damage);
		starlord.AddMember("rof", s_CharacterSettings[0].RateOfFire);
		starlord.AddMember("speed", s_CharacterSettings[0].Speed);
		starlord.AddMember("dash_speed", s_CharacterSettings[0].DashSpeed);
		starlord.AddMember("dash_distance", s_CharacterSettings[0].DashDistance);
		starlord.AddMember("dash_cooldown", s_CharacterSettings[0].DashCoolDown);
		starlord.AddMember("walk_threshold", s_CharacterSettings[0].WalkTreshold);
		starlord.AddMember("shield_regeneration", s_CharacterSettings[0].ShieldRegeneration);
		starlord.AddMember("shield_regeneration_mult", s_CharacterSettings[0].ShieldRegenerationMult);

		JSONValue rocket = doc.AddMemberObject("rocket");
		rocket.AddMember("max_health", s_CharacterSettings[1].MaxHealth);
		rocket.AddMember("max_shield", s_CharacterSettings[1].MaxShield);
		rocket.AddMember("damage", s_CharacterSettings[1].Damage);
		rocket.AddMember("rof", s_CharacterSettings[1].RateOfFire);
		rocket.AddMember("speed", s_CharacterSettings[1].Speed);
		rocket.AddMember("dash_speed", s_CharacterSettings[1].DashSpeed);
		rocket.AddMember("dash_distance", s_CharacterSettings[1].DashDistance);
		rocket.AddMember("dash_cooldown", s_CharacterSettings[1].DashCoolDown);
		rocket.AddMember("walk_threshold", s_CharacterSettings[1].WalkTreshold);
		rocket.AddMember("shield_regeneration", s_CharacterSettings[1].ShieldRegeneration);
		rocket.AddMember("shield_regeneration_mult", s_CharacterSettings[1].ShieldRegenerationMult);

		// TODO: Reset all progression
		s_PlayerInventory->Clear();

		doc.save_file("config/player_data.json");
	}

	bool GameStateManager::Continue()
	{
		if (!s_CanContinue)
			return false;

		JSONDocument doc("config/player_data.json");
		s_PlayerInventory->Deserialize(&doc);

		return true;
	}

	void GameStateManager::StartRun()
	{
		if (debug) WI_INFO("GAME STATE: StartRun()");
		SaveProgression();
		StartNewRoom();
		RandomizeRewardRoom();
		s_PlayerTriggerNext = false;

		Application::Get().GetRenderer3D().EnableSkybox(false);
	}

	void GameStateManager::EndRun()
	{
		if (debug) WI_INFO("GAME STATE: EndRun()");
		s_PlayerInventory->AddTokensHoward(PrometheanGemsToAdd);
		SetRoomType(RoomType::NONE);

		s_PlayerInventory->Clear();
		Application::Get().GetRenderer3D().EnableSkybox(true);

	}

	void GameStateManager::InitHub()
	{
		if (debug)
			WI_INFO("GAME STATE: InitHub()");

		Application::Get().GetRenderer3D().EnableSkybox(true);
		SetRoomType(RoomType::ROOM_HUB);
		SetRoomState(RoomState::STATE_FINISHED);
		InitPlayerData();
		GameMusicManager::MainHub();

		s_EnemyManager->m_CurrentCombatRoomsCount = 0;
		s_EnemyManager->ResetDifficulty();
		s_EnemyManager->Serialize();

		s_RoomsToBoss = 10;
		s_RoomsToShop = 5;
		s_CanPhalanxRooms = false;


		for (auto& scene : s_CombatRooms)
		{
			activeScenesKree.emplace_back(scene);
		}
		for (auto& scene : s_PhalanxRooms)
		{
			activeScenesPhalanx.emplace_back(scene);
		}

		deactiveScenesKree.clear();
		deactiveScenesPhalanx.clear();
	}

	void GameStateManager::InitPlayerData()
	{
		if (debug)
			WI_CORE_INFO("Init progression");
		Character* character = GetPlayerCharacterComp();
		

		if (!s_CanContinue)
			NewGame();

		JSONDocument doc("config/player_data.json");
		if (!character)
		{
			WI_ERROR("Character was nullptr");
			return; 
		}
		if (doc.IsObject())
		{
			if (doc.HasMember("starlord") && doc.HasMember("rocket"))
			{
				JSONValue characterDoc = doc["starlord"];
				if (s_CurrentCharacter == 1)
					characterDoc = doc["rocket"];

				if (characterDoc.HasMember("max_health"))
					character->MaxHealth = characterDoc["max_health"].as_int();
				character->Health = character->MaxHealth;
				if (characterDoc.HasMember("max_shield"))
					character->MaxShield = characterDoc["max_shield"].as_int();
				character->Shield = character->MaxShield;
				if (characterDoc.HasMember("shield"))
					character->Shield = characterDoc["shield"].as_int();
				if (characterDoc.HasMember("shield_regeneration"))
					character->ShieldRegeneration = characterDoc["shield_regeneration"].as_int();
				if (characterDoc.HasMember("shield_regeneration_mult"))
					character->ShieldRegenerationMult = characterDoc["shield_regeneration_mult"].as_int();
				if (characterDoc.HasMember("damage"))
					character->Damage = characterDoc["damage"].as_int();
				if (characterDoc.HasMember("rof"))
					character->RateOfFire = characterDoc["rof"].as_float();
				if (characterDoc.HasMember("speed"))
					character->Speed = characterDoc["speed"].as_float();
				if (characterDoc.HasMember("dash_distance"))
					character->DashDistance = characterDoc["dash_distance"].as_float();
				if (characterDoc.HasMember("dash_speed"))
					character->DashSpeed = characterDoc["dash_speed"].as_float();
				if (characterDoc.HasMember("dash_cooldown"))
					character->DashCooldown = characterDoc["dash_cooldown"].as_float();
				if (characterDoc.HasMember("walk_threshold"))
					character->WalkTreshold = characterDoc["walk_threshold"].as_float();
			}
		}
		//LoadPlayerAchievements(&doc);
	}

	void GameStateManager::LoadPlayerAchievements(JSONDocument* doc)
	{
		if (debug)
			WI_CORE_INFO("Init achievements loading");
		Character* character = GetPlayerCharacterComp();
		if (!character)
		{
			WI_ERROR("Character was nullptr");
			return;
		}
		if (doc->IsObject())
		{
			JSONDocument& document = *doc;
			if (doc->HasMember("Achievement"))
			{
				JSONValue achievementDoc = document["Achievement"];
				if (achievementDoc.HasMember("walk_threshold"))
					character->WalkTreshold = achievementDoc["walk_threshold"].as_float();
			}
		}
		if (debug)
			WI_CORE_INFO("Achievements init loaded");

	}

	void GameStateManager::Update()
	{
		OPTICK_EVENT("Game state manager update");
		AchievementsFunctionality();
			
		s_PlayerInventory->Update();
		s_GameProgression->Update();

		if (Input::IsKeyPressed(Key::D5)) // Hala scene
		{
			SceneManager::ChangeSceneByIndex(1);
		}

		if (Input::IsKeyPressed(Key::F5))
		{
			WI_INFO("Changing to boss");
			EndCurrentRoom();
			WI_INFO("ROOM STATE: NEXT ROOM ROOM_BOSS");
			GameStateManager::SetRoomType(RoomType::ROOM_BOSS);
			SceneManager::LoadSceneByIndex(s_BossRoomIndx);
		}
	}

	void GameStateManager::Die()
	{
		if (debug)
			WI_CORE_INFO("Player dead");	
		EndRun();
	}

	Character* GameStateManager::GetPlayerCharacterComp()
	{
		EntityManager& em = s_CurrentScene->GetEntityManager();
		EntityManager::ComponentIterator it = em.GetComponentIterator<Character>(s_PlayerId);
		return (Character*)em.GetComponentByIterator(it);
	}

	void GameStateManager::DamagePlayer(uint32_t damage)
	{
		if (s_GodMode)
		{
			WI_WARN("Can't take damage, god mode on!");
			return;
		}
		Wiwa::GuiManager& gm = s_CurrentScene->GetGuiManager();
		Character* character = GetPlayerCharacterComp();
		damage = damage / DamageDivisor;
		if (!character)
		{
			WI_CORE_CRITICAL("Player can't take damage because character is nullptr");
			return;
		}
		// The damage from this function doesn't expand to the health
		// this means that if there's remaining shield the damage doesn't apply
		// to the health even when this surpases the shield ammount
		// 
		//play voice over audio
		int random = 1 + (rand() % 100);
		if (random < 5)
		{
			// play audio
			if (GameStateManager::s_CurrentCharacter == STARLORD)
			{
				Audio::PostEvent("vo_startlord_hit");
			}
			else if (GameStateManager::s_CurrentCharacter == ROCKET)
			{
				Audio::PostEvent("vo_rocket_hit");
			}
		}

		MeshRenderer* renderer = s_CurrentScene->GetEntityManager().GetSystem<MeshRenderer>(s_PlayerId);

		// If there's no shield we take damage from the health
		if (character->Shield <= 0)
		{
			character->Health -= damage;
			if (renderer != nullptr) 
			{
				renderer->GetMaterial()->SetUniformData("u_Color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				renderer->GetMaterial()->SetUniformData("u_Hit", true);
				renderer->Update();
				renderer->GetMaterial()->SetUniformData("u_Hit", false);
			}
			Audio::PostEvent("player_hit");
			if (character->Health <= 0 && !FanaticEffect)
			{
				if (!s_GameOver)
				{
					Audio::PostEvent("player_dead");
					s_GameOver = true;
				}

				Die();
			}
			return;
		}
		
		
		if (character->Shield > 0)
		{
			character->Shield -= damage;
			Audio::PostEvent("player_hit_shield");
			if (renderer != nullptr)
			{
				renderer->GetMaterial()->SetUniformData("u_Color", glm::vec4(0, 0, 1.0f, 1.0f));
				renderer->GetMaterial()->SetUniformData("u_Hit", true);
				renderer->Update();
				renderer->GetMaterial()->SetUniformData("u_Hit", false);
			}

			EntityId player_ = GameStateManager::GetPlayerId();
			ParticleSystem* p_sys = nullptr;
			if (player_ != EntityManager::INVALID_INDEX)
			{
				EntityManager& em = SceneManager::getActiveScene()->GetEntityManager();

				EntityId p_ulti_ready_floor = em.GetChildByName(player_, "p_ulti_ready_floor");

				if (p_ulti_ready_floor != EntityManager::INVALID_INDEX)
				{
					p_sys = em.GetSystem<ParticleSystem>(p_ulti_ready_floor);

					if (p_sys != nullptr)
					{
						p_sys->SetActive(false);
					}
				}
			}
		}

		if (character->Shield <= 0)
			character->Shield = 0;
	}

	void GameStateManager::StartNewRoom()
	{
		if (debug)
			WI_INFO("GAME STATE: StartNewRoom()");
		s_PlayerTriggerNext = false;
		NextRoom();

		if (s_RoomType == RoomType::ROOM_REWARD || s_RoomType == RoomType::ROOM_SHOP || s_RoomType == RoomType::ROOM_HUB)
		{
			s_HasFinshedRoom = true;
			s_CanPassNextRoom = true;
		}
		s_GameOver = false;
	}

	void GameStateManager::SetPlayerId(EntityId id, Scene* scene)
	{

		s_PlayerId = id;
		if (id != WI_INVALID_INDEX)
		{
			s_CurrentScene = scene;
			s_CharacterStats = scene->GetEntityManager().GetComponentIterator<Character>(id);
		}
		WI_CORE_INFO("Player id set to {}", id);
	}

	void GameStateManager::EndCurrentRoom()
	{
		s_PlayerTriggerNext = false;
		if (debug) WI_INFO("GAME STATE: EndCurrentRoom()");
		if (s_RoomType == RoomType::ROOM_COMBAT)
		{
			ResetCombatRoomData();
			ResetBooleans();
			ResetWave();

		}
		ChangeRoomState(RoomState::STATE_TRANSITIONING);
		SaveProgression();

		// play audio
		if (GameStateManager::s_CurrentCharacter == STARLORD)
		{
			Audio::PostEvent("vo_starlord_complete_run");
		}
		else if (GameStateManager::s_CurrentCharacter == ROCKET)
		{
			Audio::PostEvent("vo_rocket_complete_run");
		}
	}

	void GameStateManager::LogRoomState()
	{
		WI_INFO("<-- RoomState --> {}", s_RoomStateStr[(int)s_RoomState]);
	}

	const char* GameStateManager::GetRoomState()
	{
		return s_RoomStateStr[(int)s_RoomState];
	}
	const char* GameStateManager::GetRoomType()
	{
		return s_RoomTypeStr[(int)s_RoomType];
	}

	void GameStateManager::ResetCombatRoomData()
	{
		if (debug) WI_INFO("GAME STATE: ResetCombatRoomData()");
	}

	void GameStateManager::setFinishRoom(bool value)
	{
		s_HasFinshedRoom = value;
	}

	void GameStateManager::setCanPassNextRoom(bool value)
	{
		s_CanPassNextRoom = value;
	}

	void GameStateManager::setPlayerTriggerNextRoom(bool value)
	{
		s_PlayerTriggerNext = value;
	}

	void GameStateManager::ResetBooleans()
	{
		if (debug) WI_INFO("GAME STATE: ResetBooleans()");
		s_HasFinshedRoom = false;
		s_CanPassNextRoom = false;
		s_PlayerTriggerNext = false;
	}

	int GameStateManager::NextRoom()
	{
		WI_INFO("ROOM STATE: NextRoom()");
		s_PlayerTriggerNext = false;

		RoomType type = GameStateManager::GetType();
		switch (type)
		{
		case Wiwa::RoomType::NONE:
			break;
		case Wiwa::RoomType::ROOM_HUB:
		{
			WI_INFO("ROOM STATE: NEXT ROOM COMBAT");
			GameStateManager::SetRoomType(RoomType::ROOM_COMBAT);
			GameStateManager::SetRoomState(RoomState::STATE_STARTED);
			LoadRandomCombatRoom(false);
			RandomizeRewardRoom();
			break;
		}
		case Wiwa::RoomType::ROOM_COMBAT:
		{
			WI_INFO("ROOM STATE: NEXT ROOM ROOM_REWARD");
			GameStateManager::SetRoomType(RoomType::ROOM_REWARD);
			GameStateManager::SetRoomState(RoomState::STATE_FINISHED);
			
			LoadRandomRoom(s_RewardRooms);
			RandomizeRewardRoom();

			s_EnemyManager->IncreaseRoomRewardCounter();
			break;
		}
		case Wiwa::RoomType::ROOM_REWARD:
		{
			WI_INFO("ROOM STATE: NEXT ROOM ROOM_COMBAT");
			GameStateManager::SetRoomType(RoomType::ROOM_COMBAT);
			GameStateManager::SetRoomState(RoomState::STATE_FINISHED);
			LoadRandomCombatRoom(s_CanPhalanxRooms);

			s_EnemyManager->m_CurrentCombatRoomsCount++;

			s_RoomsToBoss--;
			s_RoomsToShop--;

			if (s_RoomsToShop <= 0)
			{
				WI_INFO("ROOM STATE: NEXT ROOM ROOM_SHOP");
				GameStateManager::SetRoomType(RoomType::ROOM_SHOP);
				GameStateManager::SetRoomState(RoomState::STATE_FINISHED);
				s_RoomsToShop = 5;
				s_CanPhalanxRooms = true;
				LoadRandomRoom(s_ShopRooms);
			}
			break;
		}
		case Wiwa::RoomType::ROOM_BOSS:
		{
			SceneManager::ChangeSceneByIndex(20);
			//SceneManager::LoadSceneByIndex(s_HUBRoomIndx);
			break;
		}
		case Wiwa::RoomType::ROOM_SHOP:
		{
			if (s_RoomsToBoss <= 0)
			{
				WI_INFO("ROOM STATE: NEXT ROOM ROOM_BOSS");
				GameStateManager::SetRoomType(RoomType::ROOM_BOSS);
				SceneManager::LoadSceneByIndex(s_BossRoomIndx);
			}
			else
			{
				WI_INFO("ROOM STATE: NEXT ROOM ROOM_COMBAT");
				GameStateManager::SetRoomType(RoomType::ROOM_COMBAT);
				GameStateManager::SetRoomState(RoomState::STATE_STARTED);
				LoadRandomCombatRoom(true);
				s_EnemyManager->m_CurrentCombatRoomsCount++;
			}
			break;
		}
		default:
			break;
		}
		s_CurrentRoomsCount--;
		GetEnemyManager().Serialize();

		return 1;
	}

	int GameStateManager::LoadRandomRoom(const std::vector<int>& roomPool)
	{
		if (s_NextRewardRoomReward == 3) // Watcher
		{
			std::uniform_int_distribution<> dist(0, s_WatcherRooms.size() - 1);

			int nextRoom = dist(Application::s_Gen);
			SceneId id = s_WatcherRooms[nextRoom];
			SceneManager::ChangeSceneByIndex(id);
			return nextRoom;
		}

		std::uniform_int_distribution<> dist(0, roomPool.size() - 1);
		int nextRoom = dist(Application::s_Gen);
		SceneId id = roomPool[nextRoom];
		SceneManager::ChangeSceneByIndex(id);
		return nextRoom;
	}

	int GameStateManager::RandomizeRewardRoom()
	{
		uint32_t lastRoom = 0;
		for (size_t i = 0; i < 2; i++)
		{
			std::uniform_int_distribution<> dist(0,  100);
			
			uint32_t randomNum = 0;
			do {
				randomNum = dist(Application::s_Gen);
			} while (randomNum == lastRoom);
			
			lastRoom = randomNum;
			
			uint32_t counter = 0;

			if (IS_DROP_RATE(randomNum, counter, GameStateManager::s_ActiveSkillChances))
			{
				s_DoorsReward[i] = 0;
				continue;
			}
			counter += GameStateManager::s_ActiveSkillChances;
			
			if (IS_DROP_RATE(randomNum, counter, GameStateManager::s_PassiveSkillChances))
			{
				s_DoorsReward[i] = 1;
				continue;
			}
			counter += GameStateManager::s_PassiveSkillChances;
			
			if (IS_DROP_RATE(randomNum, counter, GameStateManager::s_BuffChances))
			{
				s_DoorsReward[i] = 2;
				continue;
			}
			counter += GameStateManager::s_BuffChances;
			
			if (IS_DROP_RATE(randomNum, counter, GameStateManager::s_NPCRoomChances))
			{
				s_DoorsReward[i] = 3;
				continue;
			}

			s_DoorsReward[i] = 0;
		}
		return 0;
	}

	void GameStateManager::CleanUp()
	{
		WI_INFO("Game state manager clean up");
		delete s_PlayerInventory;
		delete s_GameProgression;
		s_RewardRooms.clear();
		s_CombatRooms.clear();
		s_ShopRooms.clear();
		delete s_PoolManager;
		delete s_EnemyManager;
	}


	void GameStateManager::SerializeData()
	{
		JSONDocument doc;
		
		doc.AddMember("enemy_item_chance", s_EnemyDropChances);
		doc.AddMember("active_skill_chance", s_ActiveSkillChances);
		doc.AddMember("passive_skill_chance", s_PassiveSkillChances);
		doc.AddMember("buff_chance", s_BuffChances);
		doc.AddMember("npc_chance", s_NPCRoomChances);

		doc.AddMember("hub", s_HUBRoomIndx);
		doc.AddMember("boss", s_BossRoomIndx);
		doc.AddMember("intro", s_IntroductionRoom);
		JSONValue combatRooms = doc.AddMemberArray("combat");
		for (size_t i = 0; i < s_CombatRooms.size(); i++)
		{
			std::string indx = std::to_string(i);
			combatRooms.PushBack(s_CombatRooms[i]);
		}
		JSONValue phalanxRooms = doc.AddMemberArray("phalanx");
		for (size_t i = 0; i < s_PhalanxRooms.size(); i++)
		{
			std::string indx = std::to_string(i);
			phalanxRooms.PushBack(s_PhalanxRooms[i]);
		}
		JSONValue rewardRooms = doc.AddMemberArray("reward");
		for (size_t i = 0; i < s_RewardRooms.size(); i++)
		{
			std::string indx = std::to_string(i);
			rewardRooms.PushBack(s_RewardRooms[i]);
		}
		JSONValue shopRooms = doc.AddMemberArray("shop");
		for (size_t i = 0; i < s_ShopRooms.size(); i++)
		{
			std::string indx = std::to_string(i);
			shopRooms.PushBack(s_ShopRooms[i]);
		}

		JSONValue watcherRooms = doc.AddMemberArray("watchers");
		for (size_t i = 0; i < s_WatcherRooms.size(); i++)
		{
			std::string indx = std::to_string(i);
			watcherRooms.PushBack(s_WatcherRooms[i]);
		}

		JSONValue starlord = doc.AddMemberObject("starlord");
		starlord.AddMember("max_health", s_CharacterSettings[0].MaxHealth);
		starlord.AddMember("max_shield", s_CharacterSettings[0].MaxShield);
		starlord.AddMember("damage", s_CharacterSettings[0].Damage);
		starlord.AddMember("rof", s_CharacterSettings[0].RateOfFire);
		starlord.AddMember("speed", s_CharacterSettings[0].Speed);
		starlord.AddMember("dash_speed", s_CharacterSettings[0].DashSpeed);
		starlord.AddMember("dash_distance", s_CharacterSettings[0].DashDistance);
		starlord.AddMember("dash_cooldown", s_CharacterSettings[0].DashCoolDown);
		starlord.AddMember("walk_threshold", s_CharacterSettings[0].WalkTreshold);
		starlord.AddMember("shield_regeneration", s_CharacterSettings[0].ShieldRegeneration);
		starlord.AddMember("shield_regeneration_mult", s_CharacterSettings[0].ShieldRegenerationMult);

		JSONValue rocket = doc.AddMemberObject("rocket");
		rocket.AddMember("max_health", s_CharacterSettings[1].MaxHealth);
		rocket.AddMember("max_shield", s_CharacterSettings[1].MaxShield);
		rocket.AddMember("damage", s_CharacterSettings[1].Damage);
		rocket.AddMember("rof", s_CharacterSettings[1].RateOfFire);
		rocket.AddMember("speed", s_CharacterSettings[1].Speed);
		rocket.AddMember("dash_speed", s_CharacterSettings[1].DashSpeed);
		rocket.AddMember("dash_distance", s_CharacterSettings[1].DashDistance);
		rocket.AddMember("dash_cooldown", s_CharacterSettings[1].DashCoolDown);
		rocket.AddMember("walk_threshold", s_CharacterSettings[1].WalkTreshold);
		rocket.AddMember("shield_regeneration", s_CharacterSettings[1].ShieldRegeneration);
		rocket.AddMember("shield_regeneration_mult", s_CharacterSettings[1].ShieldRegenerationMult);

		doc.AddMember("gamepad_deadzone", s_GamepadDeadzone);
		doc.AddMember("current_character", s_CurrentCharacter);

		ItemManager::Serialize(&doc);

		doc.save_file("config/room_data.json");
	}

	void GameStateManager::DeserializeData()
	{
		JSONDocument doc;
		doc.load_file("config/room_data.json");
		
		if(doc.HasMember("enemy_item_chance"))
			s_EnemyDropChances = doc["enemy_item_chance"].as_int();

		if (doc.HasMember("active_skill_chance"))
			s_ActiveSkillChances = doc["active_skill_chance"].as_int();

		if (doc.HasMember("passive_skill_chance"))
			s_PassiveSkillChances = doc["passive_skill_chance"].as_int();

		if (doc.HasMember("buff_chance"))
			s_BuffChances = doc["buff_chance"].as_int();
		
		if (doc.HasMember("npc_chance"))
			s_NPCRoomChances = doc["npc_chance"].as_int();

		if (doc.HasMember("hub")) {
			s_HUBRoomIndx = doc["hub"].as_int();
		}
		if (doc.HasMember("boss"))
		{
			s_BossRoomIndx = doc["boss"].as_int();
		}
		if (doc.HasMember("combat")) {

			JSONValue scene_list = doc["combat"];

			if (scene_list.IsArray()) {
				size_t size = scene_list.Size();
				for (size_t i = 0; i < size; i++) {
					JSONValue scene = scene_list[(uint32_t)i];
					s_CombatRooms.push_back(scene.as_int());
				}
			}
		}
		if (doc.HasMember("phalanx")) {

			JSONValue scene_list = doc["phalanx"];

			if (scene_list.IsArray()) {
				size_t size = scene_list.Size();
				for (size_t i = 0; i < size; i++) {
					JSONValue scene = scene_list[(uint32_t)i];
					s_PhalanxRooms.push_back(scene.as_int());
				}
			}
		}
		if (doc.HasMember("reward")) {

			JSONValue scene_list = doc["reward"];

			if (scene_list.IsArray()) {
				size_t size = scene_list.Size();
				for (size_t i = 0; i < size; i++) {
					JSONValue scene = scene_list[(uint32_t)i];
					s_RewardRooms.push_back(scene.as_int());
				}
			}
		}

		if (doc.HasMember("shop")) {

			JSONValue scene_list = doc["shop"];

			if (scene_list.IsArray()) {
				size_t size = scene_list.Size();
				for (size_t i = 0; i < size; i++) {
					JSONValue scene = scene_list[(uint32_t)i];
					s_ShopRooms.push_back(scene.as_int());
				}
			}
		}

		if (doc.HasMember("watchers")) {

			JSONValue scene_list = doc["watchers"];

			if (scene_list.IsArray()) {
				size_t size = scene_list.Size();
				for (size_t i = 0; i < size; i++) {
					JSONValue scene = scene_list[(uint32_t)i];
					s_WatcherRooms.push_back(scene.as_int());
				}
			}
		}

		if (doc.HasMember("starlord"))
		{
			JSONValue starlord = doc["starlord"];
			s_CharacterSettings[0].MaxHealth = starlord["max_health"].as_int();
			s_CharacterSettings[0].MaxShield = starlord["max_shield"].as_int();
			s_CharacterSettings[0].Damage = starlord["damage"].as_int();
			s_CharacterSettings[0].RateOfFire = starlord["rof"].as_float();
			s_CharacterSettings[0].Speed = starlord["speed"].as_float();
			s_CharacterSettings[0].DashSpeed = starlord["dash_speed"].as_float();
			s_CharacterSettings[0].DashDistance = starlord["dash_distance"].as_float();
			s_CharacterSettings[0].DashCoolDown = starlord["dash_cooldown"].as_float();
			s_CharacterSettings[0].WalkTreshold = starlord["walk_threshold"].as_float();
			s_CharacterSettings[0].ShieldRegeneration = starlord["shield_regeneration"].as_int();
			s_CharacterSettings[0].ShieldRegenerationMult = starlord["shield_regeneration_mult"].as_int();
		}

		if (doc.HasMember("rocket"))
		{
			JSONValue rocket = doc["rocket"];
			s_CharacterSettings[1].MaxHealth = rocket["max_health"].as_int();
			s_CharacterSettings[1].MaxShield = rocket["max_shield"].as_int();
			s_CharacterSettings[1].Damage = rocket["damage"].as_int();
			s_CharacterSettings[1].RateOfFire = rocket["rof"].as_float();
			s_CharacterSettings[1].Speed = rocket["speed"].as_float();
			s_CharacterSettings[1].DashSpeed = rocket["dash_speed"].as_float();
			s_CharacterSettings[1].DashDistance = rocket["dash_distance"].as_float();
			s_CharacterSettings[1].DashCoolDown = rocket["dash_cooldown"].as_float();
			s_CharacterSettings[1].WalkTreshold = rocket["walk_threshold"].as_float();
			s_CharacterSettings[1].ShieldRegeneration = rocket["shield_regeneration"].as_int();
			s_CharacterSettings[1].ShieldRegenerationMult = rocket["shield_regeneration_mult"].as_int();
		}
		if (doc.HasMember("gamepad_deadzone"))
			s_GamepadDeadzone = doc["gamepad_deadzone"].as_float();
		if (doc.HasMember("current_character"))
			s_CurrentCharacter = doc["current_character"].as_int();

		Wiwa::ItemManager::Deserialize(&doc);

		if (std::filesystem::exists("config/player_data.json"))
		{

			JSONDocument doc("config/player_data.json");
			if (doc.IsObject())
			{
				if (doc.HasMember("starlord") && doc.HasMember("rocket"))
					s_CanContinue = true;
			}
		}

		Wiwa::GameStateManager::GetEnemyManager().DeSerialize();

		//AchievementsManager::Deserialize(&doc);
	}
	void GameStateManager::SpawnRandomItem(glm::vec3 position, uint8_t type)
	{
		
		std::string name;
		int i = 0;
		switch (type)
		{
		case 0:
		{
			std::uniform_int_distribution<> itemRandom(0, ItemManager::GetAbilities().size() - 1);
			int randomNum = itemRandom(Application::s_Gen);
			for (const auto& ability : ItemManager::GetAbilities())
			{
				if (i == randomNum)
					name = ability.second.Name;
				i++;
			}
		}break;
		case 1:
		{
			std::uniform_int_distribution<> itemRandom(0, ItemManager::GetSkills().size() - 1);
			int randomNum = itemRandom(Application::s_Gen);
			for (const auto& ability : ItemManager::GetSkills())
			{
				if (i == randomNum)
					name = ability.second.Name;
				i++;
			}
		}break;
		case 2:
		{
			std::uniform_int_distribution<> itemRandom(0, ItemManager::GetBuffs().size() - 1);
			int randomNum = itemRandom(Application::s_Gen);
			for (const auto& ability : ItemManager::GetBuffs())
			{
				if (i == randomNum)
					name = ability.second.Name;
				i++;
			}
		}break;
		case 3:
		{
			std::uniform_int_distribution<> itemRandom(0, ItemManager::GetConsumables().size() -1);
			int randomNum = itemRandom(Application::s_Gen);
			for (const auto& ability : ItemManager::GetConsumables())
			{
				if (i == randomNum)
					name = ability.second.Name;
				i++;
			}
		}break;
		default:
		{
			WI_CORE_ERROR("Item type doesn't match any type!");
			return;
		}break;
		}
		EntityManager& em = SceneManager::getActiveScene()->GetEntityManager();
		EntityId id = em.LoadPrefab("assets/Prefabs/RewardChest.wiprefab");
		EntityId childItem = em.GetChildByName(id, "Item");

		Item* item = em.GetComponent<Item>(childItem);
		Transform3D* t3d = em.GetComponent<Transform3D>(id);

		t3d->localPosition = position;
		item->item_type = type;

		name.copy(item->Name, 128);		
		item->Name[name.size()] = '\0';
		EntityId prefabParticle = Wiwa::EntityManager::INVALID_INDEX;
		Transform3D* t3d_particle = nullptr;
		ParticleSystem* particle_sytem = nullptr;

		switch (item->item_type)
		{
		case (int)ItemType::ABILITY:
			prefabParticle = em.LoadPrefab("assets/vfx/prefabs/vfx_finals/items/p_item_Active.wiprefab");
			t3d_particle = em.GetComponent<Transform3D>(prefabParticle);
			*t3d_particle = *t3d;
			em.SetParent(prefabParticle, childItem);
			t3d_particle->localPosition = { 0.0f,0.0f,0.0f };

			particle_sytem = em.GetSystem<ParticleSystem>(em.GetChildByName(prefabParticle, "icon"));

			if (strcmp(name.c_str(), "Captain Universe's Energy") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/abilities/m_capitain_universe_energy_01.wimaterial");
			}
			else if (strcmp(name.c_str(), "Groot's Seeds") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/abilities/m_groots_seeds_01.wimaterial");
			}
			else if (strcmp(name.c_str(), "Manti's Telepathic Thrust") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/abilities/m_mantis_telepathic_thrust_01.wimaterial");
			}
			else if (strcmp(name.c_str(), "Phyla's Quantum Sword") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/abilities/m_phylas_sword.wimaterial");
			}
			else if (strcmp(name.c_str(), "Starhawk's Time Blast") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/abilities/m_starhawks_time_blast_01.wimaterial");
			}
			else if (strcmp(name.c_str(), "Yondu's Fin") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/abilities/m_yondus_fin_01.wimaterial");

			}

			break;
		case (int)ItemType::BUFF:
			prefabParticle = em.LoadPrefab("assets/vfx/prefabs/vfx_finals/items/p_item_Buff.wiprefab");
			t3d_particle = em.GetComponent<Transform3D>(prefabParticle);
			*t3d_particle = *t3d;
			em.SetParent(prefabParticle, childItem);
			t3d_particle->localPosition = { 0.0f,0.0f,0.0f };

			particle_sytem = em.GetSystem<ParticleSystem>(em.GetChildByName(prefabParticle, "icon"));

			if (strcmp(name.c_str(), "Bug's Leg") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/buffs/m_bugs_legs_01.wimaterial");
			}
			else if (strcmp(name.c_str(), "Charlie-27's Fist") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/buffs/m_charlie_27s_fist_01.wimaterial");
			}
			else if (strcmp(name.c_str(), "Cosmo's Paw") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/buffs/m_cosmos_paw_01.wimaterial");
			}
			else if (strcmp(name.c_str(), "Major Victory's Shield") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/buffs/m_major_victory_shield_01.wimaterial");

			}
			else if (strcmp(name.c_str(), "Martinex's Thermokinesis") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/buffs/m_martinex_thermokinesis_01.wimaterial");

			}
			else if (strcmp(name.c_str(), "Nikki's Touch") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/buffs/m_nikkis_touch_01.wimaterial");

			}

			break;
		case (int)ItemType::PASSIVE:
			prefabParticle = em.LoadPrefab("assets/vfx/prefabs/vfx_finals/items/p_item_Passive.wiprefab");


			t3d_particle = em.GetComponent<Transform3D>(prefabParticle);
			*t3d_particle = *t3d;
			em.SetParent(prefabParticle, childItem);
			t3d_particle->localPosition = { 0.0f,0.0f,0.0f };
			particle_sytem = em.GetSystem<ParticleSystem>(em.GetChildByName(prefabParticle, "icon"));

			if (strcmp(name.c_str(), "Adam_Warlock_Blessing") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/passives/m_adam_warlock_staff_00_a.wimaterial");

			}
			else if (strcmp(name.c_str(), "Angela_Crown") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/passives/m_angelas_crown_00_a.wimaterial");

			}
			else if (strcmp(name.c_str(), "Drax_Belt") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/passives/m_drax_belt_00_a.wimaterial");

			}
			else if (strcmp(name.c_str(), "Gamora_Hood") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/passives/m_gamoras_hood_00_a.wimaterial");

			}
			else if (strcmp(name.c_str(), "Ikons_Battery") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/passives/m_ikons_battery_00_a.wimaterial");

			}
			else if (strcmp(name.c_str(), "Iron_Man_Insurance") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/passives/m_ironman_insurance_00_a.wimaterial");

			}
			else if (strcmp(name.c_str(), "Jack_Flag_Gloves") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/passives/m_jack_gloves_00_a.wimaterial");

			}
			else if (strcmp(name.c_str(), "Nova_Helmet") == 0)
			{
				particle_sytem->SetMaterial("assets/vfx/materials/icons/passives/m_nova_helmet_00_b.wimaterial");

			}
			else if (strcmp(name.c_str(), "Star_Lord_Walkman") == 0)
			{
				particle_sytem->SetMaterial("assets/vfx/materials/icons/passives/m_starlords_walkman_00_b.wimaterial");

			}
			else if (strcmp(name.c_str(), "Xandarian_Worldmind") == 0)
			{
				particle_sytem->SetMaterial("assets/vfx/materials/icons/passives/m_xandarian_worldmind_00_b.wimaterial");

			}
			break;
		default:
			break;
		}
	}
	void GameStateManager::SpawnShopRandomItem(glm::vec3 position, uint8_t type,EntityId shopItem)
	{

		std::string name;
		int i = 0;
		switch (type)
		{
		case 0:
		{
			std::uniform_int_distribution<> itemRandom(0, ItemManager::GetAbilities().size() - 1);
			int randomNum = itemRandom(Application::s_Gen);
			for (const auto& ability : ItemManager::GetAbilities())
			{
				if (i == randomNum)
					name = ability.second.Name;
				i++;
			}
		}break;
		case 1:
		{
			std::uniform_int_distribution<> itemRandom(0, ItemManager::GetAbilities().size() - 1);
			int randomNum = itemRandom(Application::s_Gen);
			for (const auto& ability : ItemManager::GetAbilities())
			{
				if (i == randomNum)
					name = ability.second.Name;
				i++;
			}

		}break;
		case 2:
		{
			std::uniform_int_distribution<> itemRandom(0, ItemManager::GetBuffs().size() -1);
			int randomNum = itemRandom(Application::s_Gen);
			for (const auto& ability : ItemManager::GetBuffs())
			{
				if (i == randomNum)
					name = ability.second.Name;
				i++;
			}
		}break;
		default:
		{
			WI_CORE_ERROR("Item type doesn't match any type!");
			return;
		}break;
		}
		EntityManager& em = SceneManager::getActiveScene()->GetEntityManager();
		em.AddComponent<Item>(shopItem);

		Item* item = em.GetComponent<Item>(shopItem);
		Transform3D* t3d = em.GetComponent<Transform3D>(shopItem);

		t3d->localPosition = position;
		item->item_type = type;
		if (item->item_type == 1)
			item->item_type = 0;

		name.copy(item->Name, 128);
		item->Name[name.size()] = '\0';

		EntityId prefabParticle = Wiwa::EntityManager::INVALID_INDEX;
		Transform3D* t3d_particle = nullptr;
		ParticleSystem* particle_sytem = nullptr;

		switch (item->item_type)
		{
		case (int)ItemType::ABILITY:
			prefabParticle = em.LoadPrefab("assets/vfx/prefabs/vfx_finals/items/p_item_Active.wiprefab");
			t3d_particle = em.GetComponent<Transform3D>(prefabParticle);
			*t3d_particle = *t3d;
			em.SetParent(prefabParticle, shopItem);
			t3d_particle->localPosition = { 0.0f,0.0f,0.0f };

			particle_sytem = em.GetSystem<ParticleSystem>(em.GetChildByName(prefabParticle, "icon"));

			if (strcmp(name.c_str(), "Captain Universe's Energy") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/abilities/m_capitain_universe_energy_01.wimaterial");
			}
			else if (strcmp(name.c_str(), "Groot's Seeds") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/abilities/m_groots_seeds_01.wimaterial");
			}
			else if (strcmp(name.c_str(), "Manti's Telepathic Thrust") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/abilities/m_mantis_telepathic_thrust_01.wimaterial");
			}
			else if (strcmp(name.c_str(), "Phyla's Quantum Sword") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/abilities/m_phylas_sword.wimaterial");
			}
			else if (strcmp(name.c_str(), "Starhawk's Time Blast") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/abilities/m_starhawks_time_blast_01.wimaterial");
			}
			else if (strcmp(name.c_str(), "Yondu's Fin") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/abilities/m_yondus_fin_01.wimaterial");

			}

			break;
		case (int)ItemType::BUFF:
			prefabParticle = em.LoadPrefab("assets/vfx/prefabs/vfx_finals/items/p_item_Buff.wiprefab");
			t3d_particle = em.GetComponent<Transform3D>(prefabParticle);
			*t3d_particle = *t3d;
			em.SetParent(prefabParticle, shopItem);
			t3d_particle->localPosition = { 0.0f,0.0f,0.0f };

			particle_sytem = em.GetSystem<ParticleSystem>(em.GetChildByName(prefabParticle, "icon"));

			if (strcmp(name.c_str(), "Bug's Leg") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/buffs/m_bugs_legs_01.wimaterial");
			}
			else if (strcmp(name.c_str(), "Charlie-27's Fist") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/buffs/m_charlie_27s_fist_01.wimaterial");
			}
			else if (strcmp(name.c_str(), "Cosmo's Paw") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/buffs/m_cosmos_paw_01.wimaterial");
			}
			else if (strcmp(name.c_str(), "Major Victory's Shield") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/buffs/m_major_victory_shield_01.wimaterial");

			}
			else if (strcmp(name.c_str(), "Martinex's Thermokinesis") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/buffs/m_martinex_thermokinesis_01.wimaterial");

			}
			else if (strcmp(name.c_str(), "Nikki's Touch") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/buffs/m_nikkis_touch_01.wimaterial");

			}

			break;
		case (int)ItemType::PASSIVE:
			prefabParticle = em.LoadPrefab("assets/vfx/prefabs/vfx_finals/items/p_item_Passive.wiprefab");
			

			t3d_particle = em.GetComponent<Transform3D>(prefabParticle);
			*t3d_particle = *t3d;
			em.SetParent(prefabParticle, shopItem);
			t3d_particle->localPosition = { 0.0f,0.0f,0.0f };
			particle_sytem = em.GetSystem<ParticleSystem>(em.GetChildByName(prefabParticle, "icon"));

			if (strcmp(name.c_str(), "Adam_Warlock_Blessing") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/passives/m_adam_warlock_staff_00_a.wimaterial");

			}
			else if (strcmp(name.c_str(), "Angela_Crown") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/passives/m_angelas_crown_00_a.wimaterial");

			}
			else if (strcmp(name.c_str(), "Drax_Belt") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/passives/m_drax_belt_00_a.wimaterial");

			}
			else if (strcmp(name.c_str(), "Gamora_Hood") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/passives/m_gamoras_hood_00_a.wimaterial");

			}
			else if (strcmp(name.c_str(), "Ikons_Battery") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/passives/m_ikons_battery_00_a.wimaterial");

			}
			else if (strcmp(name.c_str(), "Iron_Man_Insurance") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/passives/m_ironman_insurance_00_a.wimaterial");

			}
			else if (strcmp(name.c_str(), "Jack_Flag_Gloves") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/passives/m_jack_gloves_00_a.wimaterial");

			}
			else if (strcmp(name.c_str(), "Nova_Helmet") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/passives/m_nova_helmet_00_a.wimaterial");

			}
			else if (strcmp(name.c_str(), "Star_Lord_Walkman") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/passives/m_starlords_walkman_00_a.wimaterial");

			}
			else if (strcmp(name.c_str(), "Xandarian_Worldmind") == 0)
			{
				particle_sytem->SetMaterial("library/vfx/materials/icons/passives/m_xandarian_worldmind_00_a.wimaterial");

			}
			break;
		default:
			break;
		}
		
	}
	void GameStateManager::SpawnItem(glm::vec3 position, uint8_t type, const char* name)
	{
		std::string item_name = name;
		EntityManager& em = SceneManager::getActiveScene()->GetEntityManager();
		EntityId id_item = em.LoadPrefab("assets/Prefabs/Item.wiprefab");
		em.AddComponent<Item>(id_item);

		Item* item = em.GetComponent<Item>(id_item);
		Transform3D* t3d = em.GetComponent<Transform3D>(id_item);
		PhysicsSystem* pSystem = em.GetSystem<PhysicsSystem>(id_item);
		if (pSystem != nullptr)
		{
			pSystem->ForceSetPosition(position);
		}
		t3d->localPosition = position;

		item->item_type = type;

		item_name.copy(item->Name, 128);
		item->Name[item_name.size()] = '\0';

		EntityId prefabParticle = Wiwa::EntityManager::INVALID_INDEX;
		Transform3D* t3d_particle = nullptr;
		ParticleSystem* particle_sytem = nullptr;

		if (strcmp(item_name.c_str(), "Ego's Help") == 0)
		{
			prefabParticle = em.LoadPrefab("assets/vfx/prefabs/vfx_finals/items/p_item_Health.wiprefab");
			t3d_particle = em.GetComponent<Transform3D>(prefabParticle);
			*t3d_particle = *t3d;
			em.SetParent(prefabParticle, id_item);
			t3d_particle->localPosition = { 0.0f,0.0f,0.0f };
			particle_sytem = em.GetSystem<ParticleSystem>(em.GetChildByName(prefabParticle, "icon"));
			particle_sytem->SetMaterial("library/vfx/materials/icons/passives/m_egos_help_00_a.wimaterial");

		}
		else if (strcmp(item_name.c_str(), "First Aid Kit") == 0)
		{
			prefabParticle = em.LoadPrefab("assets/vfx/prefabs/vfx_finals/items/p_item_Health.wiprefab");
			t3d_particle = em.GetComponent<Transform3D>(prefabParticle);
			*t3d_particle = *t3d;
			em.SetParent(prefabParticle, id_item);
			t3d_particle->localPosition = { 0.0f,0.0f,0.0f };
			particle_sytem = em.GetSystem<ParticleSystem>(em.GetChildByName(prefabParticle, "icon"));
			particle_sytem->SetMaterial("library/vfx/materials/icons/passives/m_first_aid_kit_00_a.wimaterial");

		}
		else if (strcmp(item_name.c_str(), "Healing Pills") == 0)
		{
			prefabParticle = em.LoadPrefab("assets/vfx/prefabs/vfx_finals/items/p_item_Health.wiprefab");
			t3d_particle = em.GetComponent<Transform3D>(prefabParticle);
			*t3d_particle = *t3d;
			em.SetParent(prefabParticle, id_item);
			t3d_particle->localPosition = { 0.0f,0.0f,0.0f };
			particle_sytem = em.GetSystem<ParticleSystem>(em.GetChildByName(prefabParticle, "icon"));
			particle_sytem->SetMaterial("library/vfx/materials/icons/passives/m_healing_pills_00_a.wimaterial");

		}
		else if (strcmp(item_name.c_str(), "Medkit") == 0)
		{
			prefabParticle = em.LoadPrefab("assets/vfx/prefabs/vfx_finals/items/p_item_Health.wiprefab");
			t3d_particle = em.GetComponent<Transform3D>(prefabParticle);
			*t3d_particle = *t3d;
			em.SetParent(prefabParticle, id_item);
			t3d_particle->localPosition = { 0.0f,0.0f,0.0f };
			particle_sytem = em.GetSystem<ParticleSystem>(em.GetChildByName(prefabParticle, "icon"));
			particle_sytem->SetMaterial("library/vfx/materials/icons/passives/m_medkit_00_a.wimaterial");

		}
		else if (strcmp(item_name.c_str(), "Shield Booster") == 0)
		{
			prefabParticle = em.LoadPrefab("assets/vfx/prefabs/vfx_finals/items/p_item_Shield.wiprefab");
			t3d_particle = em.GetComponent<Transform3D>(prefabParticle);
			*t3d_particle = *t3d;
			em.SetParent(prefabParticle, id_item);
			t3d_particle->localPosition = { 0.0f,0.0f,0.0f };
			particle_sytem = em.GetSystem<ParticleSystem>(em.GetChildByName(prefabParticle, "icon"));
			particle_sytem->SetMaterial("library/vfx/materials/icons/passives/m_shield_booster_00_a.wimaterial");

		}
	}
	Transform3D* GameStateManager::GetPlayerTransform()
	{
		return s_CurrentScene->GetEntityManager().GetComponent<Transform3D>(s_PlayerId);
	}
	void GameStateManager::AchievementsFunctionality()
	{
		std::vector<ShopElement> m_ShopPassives = s_PlayerInventory->GetShopPassives();
		for (size_t i = 0; i < m_ShopPassives.size(); i++)
		{
			Wiwa::HowardElementType type_howard = m_ShopPassives.at(i).PassiveBoost;
			switch (type_howard)
			{
			case Wiwa::HowardElementType::RECOVERY_SHIELD:
				if(s_RoomState == Wiwa::RoomState::STATE_AWAITING_NEXT)
					m_ShopPassives.at(i).Use();
				break;
			case Wiwa::HowardElementType::SECOND_WIND:

				break;
			case Wiwa::HowardElementType::REROLL:
				break;
			case Wiwa::HowardElementType::BEGINNERS_LUCK:
				break;
			case Wiwa::HowardElementType::MIDAS_TOUCH:
				break;
			case Wiwa::HowardElementType::DEVOURER:
				break;
			case Wiwa::HowardElementType::FANATIC:
				if (FanaticEffect)
				{
					Character* character = GetPlayerCharacterComp();

					if (character->Health <= 0)
					{
						character->Health = 100;
						FanaticEffect = false;
					}
				}
				break;
			case Wiwa::HowardElementType::RECOVERY_HEALTH:
				if (s_RoomState == Wiwa::RoomState::STATE_AWAITING_NEXT)
					m_ShopPassives.at(i).Use();
				break;
			case Wiwa::HowardElementType::ULTIMATE_MIDAS_TOUCH:
				break;
			case Wiwa::HowardElementType::FRIENDLY_FACE:
				break;
			default:
				break;
			}
		}
	}

	int GameStateManager::GetActiveEnemies()
	{
		EntityManager& em = SceneManager::getActiveScene()->GetEntityManager();
		int total = 0;

		// Get the first and only spawner in scene
		size_t size = 0;
		Wiwa::WaveSpawner* waveSpawner = nullptr;
		waveSpawner = em.GetComponents<WaveSpawner>(&size);
		if (waveSpawner) {
			if (em.IsComponentRemoved<WaveSpawner>(0))
				return 0;
			waveSpawner = &waveSpawner[0];
			if (waveSpawner && waveSpawner->hasTriggered) {
				// Check for all the active waves in that spawner.
				WaveSpawnerSystem* waveSpawnerSystem = em.GetSystem<WaveSpawnerSystem>(waveSpawner->entityId);
				if (waveSpawnerSystem) {
					const std::vector<EntityId>& waveIds = waveSpawnerSystem->getWaveIds();
					for (int i = 0; i < waveIds.size(); i++)
					{
						Wave* wave = em.GetComponent<Wave>(waveIds[i]);
						if (wave && !wave->hasFinished) {
							total += wave->currentEnemiesAlive;
						}
					}
				}
			}
		}

		return total;
	}

	int GameStateManager::GetAproximateTotalEnemies()
	{
		EntityManager& em = SceneManager::getActiveScene()->GetEntityManager();
		int total = 0;

		// Get the first and only spawner in scene
		size_t size = 0;
		Wiwa::WaveSpawner* waveSpawner = nullptr;
		waveSpawner = em.GetComponents<WaveSpawner>(&size);
		if (waveSpawner) {
			if (em.IsComponentRemoved<WaveSpawner>(0))
				return 0;
			waveSpawner = &waveSpawner[0];
			if (waveSpawner && waveSpawner->hasTriggered) {
				// Check for all the active waves in that spawner.
				WaveSpawnerSystem* waveSpawnerSystem = em.GetSystem<WaveSpawnerSystem>(waveSpawner->entityId);
				if (waveSpawnerSystem) {
					const std::vector<EntityId>& waveIds = waveSpawnerSystem->getWaveIds();
					for (int i = 0; i < waveIds.size(); i++)
					{
						Wave* wave = em.GetComponent<Wave>(waveIds[i]);
						if (wave) {
							total += wave->maxEnemies;
						}
					}
				}
			}
		}

		return total;
	}

	//int GameStateManager::GetTotalWaves()
	//{
	//	EntityManager& em = SceneManager::getActiveScene()->GetEntityManager();
	//	int zero = 0;

	//	// Get the first and only spawner in scene
	//	size_t size = 0;
	//	Wiwa::WaveSpawner* waveSpawner = nullptr;
	//	waveSpawner = em.GetComponents<WaveSpawner>(&size);
	//	if (waveSpawner) {
	//		if (em.IsComponentRemoved<WaveSpawner>(0))
	//			return zero;
	//		waveSpawner = &waveSpawner[0];
	//		if (waveSpawner && waveSpawner->hasTriggered) {
	//			return waveSpawner->maxWaveCount;
	//		}
	//	}

	//	return zero;
	//}

	//int GameStateManager::GetCurrentWaves()
	//{
	//	EntityManager& em = SceneManager::getActiveScene()->GetEntityManager();
	//	int count = 0;

	//	size_t size = 0;
	//	Wiwa::WaveSpawner* enemySpawnerList = nullptr;
	//	enemySpawnerList = em.GetComponents<WaveSpawner>(&size);
	//	if (enemySpawnerList) {
	//		for (int i = 0; i < size; i++) {
	//			if (em.IsComponentRemoved<WaveSpawner>(i)) {
	//			}
	//			else {
	//				Wiwa::WaveSpawner* waveSpawner = &enemySpawnerList[i];
	//				if (waveSpawner) {
	//					WaveSpawnerSystem* waveSpawnerSystem = em.GetSystem<WaveSpawnerSystem>(waveSpawner->entityId);
	//					if (waveSpawnerSystem) {
	//						const std::vector<EntityId>& waveIds = waveSpawnerSystem->getWaveIds();
	//						for (int i = 0; i < waveIds.size(); i++)
	//						{
	//							Wave* wave = em.GetComponent<Wave>(waveIds[i]);
	//							if (wave) {
	//								count++;
	//							}
	//						}
	//					}
	//				}
	//			}
	//		}
	//	}

	//	return count;
	//}

	bool GameStateManager::IsWaveSpawnerFinished(WaveSpawner* waveSpawner)
	{
		EntityManager& em = SceneManager::getActiveScene()->GetEntityManager();
		if (waveSpawner && !waveSpawner->hasFinished) {
			// Check for all the active waves in that spawner.
			WaveSpawnerSystem* waveSpawnerSystem = em.GetSystem<WaveSpawnerSystem>(waveSpawner->entityId);
			if (waveSpawnerSystem) {
				const std::vector<EntityId>& waveIds = waveSpawnerSystem->getWaveIds();
				int count = 0;
				for (int i = 0; i < waveIds.size(); i++)
				{
					Wave* wave = em.GetComponent<Wave>(waveIds[i]);
					if (wave) {
						if (!wave->hasFinished)
							return false;
					}
				}
			}
		}
		
		if (waveSpawner && waveSpawner->hasTriggered) {
			WaveSpawnerSystem* waveSpawnerSystem = em.GetSystem<WaveSpawnerSystem>(waveSpawner->entityId);
			if (waveSpawnerSystem) {
				const std::vector<EntityId>& waveIds = waveSpawnerSystem->getWaveIds();
				int count = 0;
				for (int i = 0; i < waveIds.size(); i++)
				{
					Wave* wave = em.GetComponent<Wave>(waveIds[i]);
					if (wave) {
						if (!wave->hasFinished)
							return false;
						else
							count += 1;
					}
				}

				if (waveSpawner->hasFinished && count == waveIds.size())
					return true;
			}
		}
	

		return false;
	}

	void GameStateManager::EndCombatRoom(WaveSpawner* waveSpawner)
	{
		EntityManager& em = SceneManager::getActiveScene()->GetEntityManager();
		if (waveSpawner && !waveSpawner->hasFinished) {
			// Check for all the active waves in that spawner.
			WaveSpawnerSystem* waveSpawnerSystem = em.GetSystem<WaveSpawnerSystem>(waveSpawner->entityId);
			if (waveSpawnerSystem) {
				const std::vector<EntityId>& waveIds = waveSpawnerSystem->getWaveIds();
				for (int i = 0; i < waveIds.size(); i++)
				{
					Wave* wave = em.GetComponent<Wave>(waveIds[i]);
					if (wave) {

						WaveSystem* waveSystem = em.GetSystem<WaveSystem>(waveIds[i]);
						if (waveSystem) {
							const std::vector<EntityId>& enemiesIds = waveSystem->getEnemiesIds();
							for (int j = 0; j < enemiesIds.size(); j++)
							{
								em.DestroyEntity(enemiesIds[j]);
								waveSystem->DestroyEnemy(enemiesIds[j]);
							}
						}

						wave->hasFinished = true;
					}
				}
			}
			waveSpawner->hasFinished = true;
		}
	}
}


