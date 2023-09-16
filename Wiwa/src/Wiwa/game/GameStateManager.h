#pragma once

#include <Wiwa/core/Core.h>

#include <Wiwa/ecs/EntityManager.h>
#include <vector>
#include "Items/Inventory.h"
#include <Wiwa/game/GameProgression.h>
// #include <Wiwa/ecs/components/game/Character.h>
// #include <Wiwa/ecs/components/game/wave/Wave.h>
#include <Wiwa/ecs/components/game/wave/WaveSpawner.h>
#include <Wiwa/game/Pooling/GamePoolingManager.h>
#include "Enemy/GameEnemyManager.h"

typedef size_t SceneId;
#define STARLORD 0
#define ROCKET 1
namespace Wiwa
{
	class GamePoolingManager;
	class AchivementsManager;
	struct DefaultCharacterSettings
	{
		int MaxHealth;
		int MaxShield;

		int Damage;
		float RateOfFire;

		float Speed;
		float DashDistance;
		float DashSpeed;
		float DashCoolDown;
		float WalkTreshold;

		int ShieldRegeneration;
		int ShieldRegenerationMult;
	};

	enum class RoomType
	{
		NONE = 0,
		ROOM_HUB = 1,
		ROOM_COMBAT = 2,
		ROOM_REWARD = 3,
		ROOM_BOSS = 4,
		ROOM_SHOP = 5
	};
	static const char *s_RoomTypeStr[] =
		{"NONE", "ROOM_HUB", "ROOM_COMBAT", "ROOM_REWARD", "ROOM_BOSS", "ROOM_SHOP"};

	enum class RoomState
	{
		NONE = 0,
		STATE_STARTED = 1,
		STATE_FINISHED = 2,
		STATE_AWAITING_NEXT = 3,
		STATE_TRANSITIONING = 4
	};
	static const char *s_RoomStateStr[] =
		{"NONE", "STATE_STARTED", "STATE_FINISHED", "STATE_AWAITING_NEXT", "STATE_TRANSITIONING"};

	enum class ItemType : uint8_t
	{
		ABILITY = 0,
		PASSIVE = 1,
		BUFF = 2,
		CONSUMABLE = 3
	};

	class WI_API GameStateManager
	{
	private:
		static RoomType s_RoomType;
		static RoomState s_RoomState;
		static int s_CurrentWave;
		static void ChangeRoomState(RoomState room_state);

	public:
		static bool s_HasFinshedRoom;
		static bool s_CanPassNextRoom;
		static bool s_PlayerTriggerNext;
		static bool debug;
		static bool s_GameOver;

		// Save & Load Overall Player Progression inside the run
		static void SaveProgression();
		static void LoadProgression();

		static void UpdateRoomState();
		static void UpdateCombatRoom();

		// Run and rooms
		static void NewGame();
		static bool Continue();

		static void StartRun();
		static void EndRun();
		static void InitHub();
		static void InitPlayerData();
		static void LoadPlayerAchievements(JSONDocument *doc);

		static void Update();
		static void Die();
		static struct Character *GetPlayerCharacterComp();
		static void DamagePlayer(uint32_t damage);
		static void StartNewRoom();
		static void SetPlayerId(EntityId id, Scene *scene);
		static void EndCurrentRoom();

		WI_HARD_INL static void SetEnemyDropChance(int newDropChance) { s_EnemyDropChances = newDropChance; }
		static void SetRoomType(RoomType type) 
		{
			s_RoomType = type;
			if(s_RoomType == RoomType::ROOM_BOSS)
				s_PlayerInventory->AddTokensHoward(PrometheanGemsToAdd);
		}
		WI_HARD_INL static void SetRoomState(RoomState type) { s_RoomState = type; }
		WI_HARD_INL static RoomType GetType() { return s_RoomType; }
		WI_HARD_INL static RoomState GetState() { return s_RoomState; }

		static void LogRoomState();

		static const char *GetRoomState();
		static const char *GetRoomType();
		// Reset combat data
		static void ResetCombatRoomData();
		static void setFinishRoom(bool value);
		static void setCanPassNextRoom(bool value);
		static void setPlayerTriggerNextRoom(bool value);

		// Reset general data
		static void ResetBooleans();

		static int NextRoom();

		static int LoadRandomRoom(const std::vector<int> &roomPool);

		static int RandomizeRewardRoom();

		static void CleanUp();

		static void SerializeData();
		static void DeserializeData();

		/// <summary>
		/// Spawns a random item of a given type in a given location
		/// </summary>
		/// <param name="position"></param>
		/// <param name="type">0 == ability, 1 == passive, 2 == buff, 3 == consumable</param>
		static void SpawnRandomItem(glm::vec3 position, uint8_t type);
		/// <summary>
		/// Spawns a random item of a given type in a given location, instead of using the chest prefab the item is displayed with another prefab
		/// </summary>
		/// <param name="position"></param>
		/// <param name="type">0 == ability, 1 == passive, 2 == buff, 3 == consumable</param>
		static void SpawnShopRandomItem(glm::vec3 position, uint8_t type,EntityId shopItem);
		/// <summary>
		/// Spawns a given item of a given type in a given location
		/// </summary>
		/// <param name="position"></param>
		/// <param name="type">0 == ability, 1 == passive, 2 == buff, 3 == consumable</param>
		/// <param name="name"></param>
		static void SpawnItem(glm::vec3 position, uint8_t type, const char* name);

		WI_HARD_INL static struct Transform3D *GetPlayerTransform();
		WI_HARD_INL static Inventory &GetPlayerInventory() { return *s_PlayerInventory; }
		WI_HARD_INL static Scene *GetCurrentScene() { return s_CurrentScene; }
		WI_HARD_INL static EntityId GetPlayerId() { return s_PlayerId; }
		WI_HARD_INL static float GetControllerDeadZone() { return s_GamepadDeadzone; }
		WI_HARD_INL static EnemyManager &GetEnemyManager() { return *s_EnemyManager; }

		// Achievement items progression
		static void AchievementsFunctionality();
		static void SetFanaticEffect(bool ret) { FanaticEffect = ret; }
		static void IncrementPrometheanGems(int amount) { PrometheanGemsToAdd += amount; }
		static bool ReturnFanaticEffect() { return FanaticEffect; }

		static void IncrementCurrentWave() { s_CurrentWave += 1; }
		static int GetCurrentWave() { return s_CurrentWave; }
		static void ResetWave() { s_CurrentWave = 0; }
	public:
		static DefaultCharacterSettings s_CharacterSettings[2];
		static EntityManager::ComponentIterator s_CharacterStats;
		static int s_CurrentCharacter;
		static float s_GamepadDeadzone;
		static int s_RoomsToShop;
		static int s_RoomsToBoss;

		static SceneId s_BossRoomIndx;
		static SceneId s_HUBRoomIndx;
		static SceneId s_IntroductionRoom;
		static SceneId s_CurrentRoomIndx;
		static SceneId s_LastCombatRoom;
		static SceneId s_LastRewardRoom;
		static SceneId s_LastShopRoom;
		static bool s_CanPhalanxRooms;
		static std::vector<int> s_CombatRooms;
		static std::vector<int> s_PhalanxRooms;
		static std::vector<int> s_RewardRooms;
		static std::vector<int> s_ShopRooms;
		static std::vector<int> s_WatcherRooms;

		static int s_CurrentRoomsCount;

		static EntityId s_PlayerId;

		static Scene *s_CurrentScene;

		static Inventory *s_PlayerInventory;

		static GameProgression *s_GameProgression;
		static AchivementsManager *s_AchivementsManager;

		static int s_EnemyDropChances;
		static int s_ActiveSkillChances;
		static int s_BuffChances;
		static int s_PassiveSkillChances;
		static int s_NPCRoomChances;

		static bool FanaticEffect;
		static bool SecondWind;
		static int PrometheanGemsToAdd;
		static int DamageDivisor;

		static GamePoolingManager *s_PoolManager;

		static int s_NextRewardRoomReward;
		static int s_DoorsReward[2];
		static bool s_CanContinue;
		static EnemyManager *s_EnemyManager;

		// Enemies
		public:
		static int GetActiveEnemies();
		static int GetAproximateTotalEnemies();
		
		static bool IsWaveSpawnerFinished(WaveSpawner* waveSpawner);
		static void EndCombatRoom(WaveSpawner* waveSpawner);
		
		static bool s_GodMode;
	};
}