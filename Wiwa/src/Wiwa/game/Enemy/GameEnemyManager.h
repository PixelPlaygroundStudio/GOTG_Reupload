#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <Wiwa/ecs/components/game/enemy/EnemyData.h>
#include <Wiwa/ecs/components/game/enemy/PhalanxRanged.h>
#include <Wiwa/ecs/components/game/enemy/Subjugator.h>
#include <Wiwa/ecs/components/game/enemy/Ultron.h>
#include <Wiwa/ecs/components/game/wave/WaveSpawner.h>

namespace Wiwa
{
	struct VariantData
	{
		int n;
		std::map<std::string, bool> list;
	};

	struct SpawnerData
	{
		int n;
		WaveSpawner waveSpawnData;
	};

	class WI_API EnemyManager
	{
	public:
		EnemyManager();
		~EnemyManager();

		void CleanUp();
		bool Serialize();
		bool DeSerialize();
		bool CreateEmptyFile();
		void Reset();

		bool CheckIfHasLevelCreate(int level, const std::string& str);
		bool CheckIfHasLevelDelete(int level, const std::string& str);

		// Level difficulty managing
		int ResetDifficulty();
		int IncreaseDifficulty();
		int DecreaseDifficulty();
		int IncreaseRoomRewardCounter();

		inline std::map<std::pair<int, std::string>, EnemyData>& GetData() { return m_EnemyData; };
		std::map<std::pair<int, std::string>, EnemyData> m_EnemyData;
		std::vector<VariantData> m_VariantsTable;
		std::vector<SpawnerData> m_SpawnerDataTable;

		PhalanxRanged m_RangedData;
		Subjugator m_SubjugatorData;
		Ultron m_UltronData;

		const int m_BaseLevel = 0;
		int m_MaxLevel = 4;
		int m_CurrentRunLevel = 0;
		int m_IncreaseDiffEvery = 1;
		int m_CurrentRewardRoomCount = 0;
		int m_CurrentCombatRoomsCount = 0;
	};
}