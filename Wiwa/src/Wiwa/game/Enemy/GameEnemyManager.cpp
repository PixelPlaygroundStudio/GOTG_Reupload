#include <wipch.h>
#include "GameEnemyManager.h"
#include <Wiwa/utilities/Log.h>
#include "json.hpp"

namespace nlohmann {
    template <>
    struct adl_serializer<Wiwa::EnemyData> {
        static void to_json(json& j, const Wiwa::EnemyData& e)
        {
            j = json{ {"level", e.level}, {"name", e.name}, {"health", e.health}, {"damage", e.damage}, {"maxVelocity", e.maxVelocity}, 
				{"maxAcceleration", e.maxAcceleration}, {"stoppingDistance", e.stoppingDistance}, {"rateOfFire", e.rateOfFire},  {"range", e.range}, {"creditsDrop", e.creditsDrop} };
        }

        static void from_json(const json& j, Wiwa::EnemyData& e)
        {
            j.at("level").get_to(e.level);
            j.at("name").get_to(e.name);
            j.at("health").get_to(e.health);
            j.at("damage").get_to(e.damage);
            j.at("maxVelocity").get_to(e.maxVelocity);
            j.at("maxAcceleration").get_to(e.maxAcceleration);
            j.at("stoppingDistance").get_to(e.stoppingDistance);
            j.at("rateOfFire").get_to(e.rateOfFire);
            j.at("range").get_to(e.range);
            j.at("creditsDrop").get_to(e.creditsDrop);
        }
    };

    template <>
    struct adl_serializer<std::map<std::pair<int, std::string>, Wiwa::EnemyData>> {
        static void to_json(json& j, const std::map<std::pair<int, std::string>, Wiwa::EnemyData>& m)
        {
            for (const auto& [key, value] : m) {
                j[std::to_string(key.first) + "_" + key.second] = value;
            }
        }

        static void from_json(const json& j, std::map<std::pair<int, std::string>, Wiwa::EnemyData>& m)
        {
            for (const auto& [key, value] : j.items()) {
                auto underscore_pos = key.find_first_of('_');
                int first = std::stoi(key.substr(0, underscore_pos));
                std::string second = key.substr(underscore_pos + 1);
                m[std::make_pair(first, second)] = value.get<Wiwa::EnemyData>();
            }
        }
    };

	template <>
	struct adl_serializer<Wiwa::VariantData> {
		static void to_json(json& j, const Wiwa::VariantData& e)
		{
			j = json{ {"n", e.n},
				{"list", e.list} };
		}

		static void from_json(const json& j, Wiwa::VariantData& e)
		{
			j.at("n").get_to(e.n);
			j.at("list").get_to(e.list);
		}
	};

	template <>
	struct adl_serializer<Wiwa::WaveSpawner> {
		static void to_json(json& j, const Wiwa::WaveSpawner& e)
		{
			j = json{ {"maxEnemiesPerWave", e.maxEnemiesPerWave},
				{"minEnemiesPerWave", e.minEnemiesPerWave},
				{"enemySpawnRate", e.enemySpawnRate},
				{"maxWaveCount", e.maxWaveCount},
				{"timeBetweenWaves", e.timeBetweenWaves},
				{"waveChangeRate", e.waveChangeRate},
			};
		}

		static void from_json(const json& j, Wiwa::WaveSpawner& e)
		{
			j.at("maxEnemiesPerWave").get_to(e.maxEnemiesPerWave);
			j.at("minEnemiesPerWave").get_to(e.minEnemiesPerWave);
			j.at("enemySpawnRate").get_to(e.enemySpawnRate);
			j.at("maxWaveCount").get_to(e.maxWaveCount);
			j.at("timeBetweenWaves").get_to(e.timeBetweenWaves);
			j.at("waveChangeRate").get_to(e.waveChangeRate);
		}
	};

	template <>
	struct adl_serializer<Wiwa::SpawnerData> {
		static void to_json(json& j, const Wiwa::SpawnerData& e)
		{
			j = json{ {"n", e.n},
				{"waveSpawnData", e.waveSpawnData} };
		}

		static void from_json(const json& j, Wiwa::SpawnerData& e)
		{
			j.at("n").get_to(e.n);
			j.at("waveSpawnData").get_to(e.waveSpawnData);
		}
	};
}

void to_json(nlohmann::json& j, const Wiwa::Ultron& u) {
	j = nlohmann::json{
		{"bulletStorm_firstPattern_bulletsPerWave", u.bulletStorm_firstPattern_bulletsPerWave},
		{"bulletStorm_firstPattern_waves", u.bulletStorm_firstPattern_waves},
		{"bulletStorm_secondPattern_bulletsPerSpiral", u.bulletStorm_secondPattern_bulletsPerSpiral},
		{"bulletStorm_secondPattern_timerBetweenSpirals", u.bulletStorm_secondPattern_timerBetweenSpirals},
		{"bulletStorm_secondPattern_spirals", u.bulletStorm_secondPattern_spirals},
		{"bulletStorm_thirdPattern_bulletsPerCross", u.bulletStorm_thirdPattern_bulletsPerCross},
		{"bulletStorm_thirdPattern_timerBetweenCross", u.bulletStorm_thirdPattern_timerBetweenCross},
		{"bulletStorm_thirdPattern_crosses", u.bulletStorm_thirdPattern_crosses},
		{"clusterShot_bullets", u.clusterShot_bullets},
		{"clusterShot_timerBetweenBullets", u.clusterShot_timerBetweenBullets},
		{"clusterShot_miniBullets", u.clusterShot_miniBullets},
		{"laserBeam_tickDamage", u.laserBeam_tickDamage},
		{"laserBeam_tickTimer", u.laserBeam_tickTimer},
		{"laserBeam_duration", u.laserBeam_duration},
	};
}

void from_json(const nlohmann::json& j, Wiwa::Ultron& u) {
	u.bulletStorm_firstPattern_bulletsPerWave = j.value("bulletStorm_firstPattern_bulletsPerWave", 8);
	u.bulletStorm_firstPattern_waves = j.value("bulletStorm_firstPattern_waves", 4);

	u.bulletStorm_secondPattern_bulletsPerSpiral = j.value("bulletStorm_secondPattern_bulletsPerSpiral", 8);
	u.bulletStorm_secondPattern_timerBetweenSpirals = j.value("bulletStorm_secondPattern_timerBetweenSpirals", 0.2f);
	u.bulletStorm_secondPattern_spirals = j.value("bulletStorm_secondPattern_spirals", 3);

	u.bulletStorm_thirdPattern_bulletsPerCross = j.value("bulletStorm_thirdPattern_bulletsPerCross", 8);
	u.bulletStorm_thirdPattern_timerBetweenCross = j.value("bulletStorm_thirdPattern_timerBetweenCross", 0.2f);
	u.bulletStorm_thirdPattern_crosses = j.value("bulletStorm_thirdPattern_crosses", 3);

	u.clusterShot_bullets = j.value("clusterShot_bullets", 3);
	u.clusterShot_timerBetweenBullets = j.value("clusterShot_timerBetweenBullets", 2.0f);
	u.clusterShot_miniBullets = j.value("clusterShot_miniBullets", 8);

	u.laserBeam_tickDamage = j.value("laserBeam_tickDamage", 20);
	u.laserBeam_tickTimer = j.value("laserBeam_tickTimer", 1.0f);
	u.laserBeam_duration = j.value("laserBeam_duration", 5);
}

struct MapKeyComparator {
	bool operator()(const std::pair<int, std::string>& a, const std::pair<int, std::string>& b) const {
		if (a.second != b.second) {
			return a.second < b.second; // sort by string first
		}
		return a.first < b.first; // then sort by int
	}
};

using json = nlohmann::json;

namespace Wiwa
{
	EnemyManager::EnemyManager()
	{
		m_MaxLevel = 2;
		m_CurrentRunLevel = 0;
		m_IncreaseDiffEvery = 1;
		m_CurrentRewardRoomCount = 0;
		m_CurrentCombatRoomsCount = 0;

		m_RangedData.bulletLifeTime = 6.0f;
		m_RangedData.bulletSpeed = 30.0f;
			
		m_SubjugatorData.bulletLifeTime = 6.0f;
		m_SubjugatorData.bulletSpeed = 30.0f;

		m_UltronData.bulletLifeTime = 6.0f;
		m_UltronData.bulletSpeed = 30.0f;
	}
	EnemyManager::~EnemyManager()
	{
	}
	void EnemyManager::CleanUp()
	{
	}
	
	bool EnemyManager::Serialize()
	{
		std::filesystem::path path;
		path = "config/enemy_data.json";

		json doc;
		doc["m_EnemyData"] = m_EnemyData;
		doc["m_MaxLevel"] = m_MaxLevel;
		doc["m_CurrentRunLevel"] = m_CurrentRunLevel;
		doc["m_IncreaseDiffEvery"] = m_IncreaseDiffEvery;
		doc["m_CurrentCombatRoomsCount"] = m_CurrentCombatRoomsCount;
		doc["m_CurrentRewardRoomCount"] = m_CurrentRewardRoomCount;

		doc["m_RangedData"]["bulletLifeTime"] = m_RangedData.bulletLifeTime;
		doc["m_RangedData"]["bulletSpeed"] = m_RangedData.bulletSpeed;

		doc["m_SubjugatorData"]["bulletLifeTime"] = m_SubjugatorData.bulletLifeTime;
		doc["m_SubjugatorData"]["bulletSpeed"] = m_SubjugatorData.bulletSpeed;

		doc["m_UltronData"]["bulletLifeTime"] = m_UltronData.bulletLifeTime;
		doc["m_UltronData"]["bulletSpeed"] = m_UltronData.bulletSpeed;

		doc["m_VariantsTable"] = m_VariantsTable;
		doc["m_SpawnerDataTable"] = m_SpawnerDataTable;

		std::ofstream file("config/enemy_data.json");
		if (file.is_open())
		{
			file << doc;
			file.close();
			WI_INFO("Enemy stats saved to enemy_stats.json");
			return true;
		}

		WI_INFO("Error: Could not open config/enemy_data.json for writing");
		return false;
	}

	bool EnemyManager::DeSerialize()
	{
		std::filesystem::path path;
		path = "config/enemy_data.json";
		bool exists = std::filesystem::exists(path);

		if (!exists)
			CreateEmptyFile();

		std::ifstream file(path);
		json serializedData;
		if (file.is_open())
		{
			std::string data((std::istreambuf_iterator<char>(file)),
				std::istreambuf_iterator<char>());

			serializedData = json::parse(data);

			m_EnemyData = serializedData["m_EnemyData"].get<std::map<std::pair<int, std::string>, EnemyData>>();
			m_MaxLevel = serializedData["m_MaxLevel"].get<int>();
			m_CurrentRunLevel = serializedData["m_CurrentRunLevel"].get<int>();
			m_IncreaseDiffEvery = serializedData["m_IncreaseDiffEvery"].get<int>();
			m_CurrentCombatRoomsCount = serializedData["m_CurrentCombatRoomsCount"].get<int>();
			m_CurrentRewardRoomCount = serializedData["m_CurrentRewardRoomCount"].get<int>();

			m_RangedData.bulletLifeTime = serializedData["m_RangedData"]["bulletLifeTime"].get<float>();
			m_RangedData.bulletSpeed = serializedData["m_RangedData"]["bulletSpeed"].get<float>();

			m_SubjugatorData.bulletLifeTime = serializedData["m_SubjugatorData"]["bulletLifeTime"].get<float>();
			m_SubjugatorData.bulletSpeed = serializedData["m_SubjugatorData"]["bulletSpeed"].get<float>();

			m_UltronData.bulletLifeTime = serializedData["m_UltronData"]["bulletLifeTime"].get<float>();
			m_UltronData.bulletSpeed = serializedData["m_UltronData"]["bulletSpeed"].get<float>();

			m_VariantsTable = serializedData["m_VariantsTable"].get<std::vector<VariantData>>();
			m_SpawnerDataTable = serializedData["m_SpawnerDataTable"].get<std::vector<SpawnerData>>();

			file.close();
			WI_INFO("Enemy stats LOADED FROM enemy_stats.json");
			return true;
		}

		return false;
	}

	bool EnemyManager::CreateEmptyFile()
	{
		VariantData variantData;

		// Load base stats melee phalanx generic
		EnemyData meleePhalanxGeneric;
		meleePhalanxGeneric.name = "MELEE_PHALANX_GENERIC";
		meleePhalanxGeneric.health = 30;
		meleePhalanxGeneric.damage = 10;
		meleePhalanxGeneric.maxVelocity = 15;
		meleePhalanxGeneric.maxAcceleration = 15;
		meleePhalanxGeneric.stoppingDistance = 20;
		meleePhalanxGeneric.rateOfFire = 2;
		meleePhalanxGeneric.range = 1;
		meleePhalanxGeneric.creditsDrop = 15;
		meleePhalanxGeneric.level = 0;
		m_EnemyData[{meleePhalanxGeneric.level, meleePhalanxGeneric.name.c_str()}] = meleePhalanxGeneric;
		variantData.list["MELEE_PHALANX_GENERIC"] = true;

		// Load base stats melee phalanx RedVariant
		EnemyData meleePhalanxRedVariant;
		meleePhalanxRedVariant.name = "MELEE_PHALANX_REDVARIANT";
		meleePhalanxRedVariant.health = 70;
		meleePhalanxRedVariant.damage = 20;
		meleePhalanxRedVariant.maxVelocity = 15;
		meleePhalanxRedVariant.maxAcceleration = 15;
		meleePhalanxRedVariant.stoppingDistance = 20;
		meleePhalanxRedVariant.rateOfFire = 2;
		meleePhalanxRedVariant.range = 1;
		meleePhalanxRedVariant.creditsDrop = 20;
		meleePhalanxRedVariant.level = 0;
		m_EnemyData[{meleePhalanxRedVariant.level, meleePhalanxRedVariant.name.c_str()}] = meleePhalanxRedVariant;
		variantData.list["MELEE_PHALANX_REDVARIANT"] = true;

		// Load base stats melee phalanx BlueVariant
		EnemyData meleePhalanxBlueVariant;
		meleePhalanxBlueVariant.name = "MELEE_PHALANX_BLUEVARIANT";
		meleePhalanxBlueVariant.health = 20;
		meleePhalanxBlueVariant.damage = 20;
		meleePhalanxBlueVariant.maxVelocity = 15;
		meleePhalanxBlueVariant.maxAcceleration = 15;
		meleePhalanxBlueVariant.stoppingDistance = 20;
		meleePhalanxBlueVariant.rateOfFire = 1;
		meleePhalanxBlueVariant.range = 1;
		meleePhalanxBlueVariant.creditsDrop = 20;
		meleePhalanxBlueVariant.level = 0;
		m_EnemyData[{meleePhalanxBlueVariant.level, meleePhalanxBlueVariant.name.c_str()}] = meleePhalanxBlueVariant;
		variantData.list["MELEE_PHALANX_BLUEVARIANT"] = true;

		// Load base stats ranged phalanx generic
		EnemyData rangedPhalanxGeneric;
		rangedPhalanxGeneric.name = "RANGED_PHALANX_GENERIC";
		rangedPhalanxGeneric.health = 20;
		rangedPhalanxGeneric.damage = 5;
		rangedPhalanxGeneric.maxVelocity = 8;
		rangedPhalanxGeneric.maxAcceleration = 8;
		rangedPhalanxGeneric.stoppingDistance = 2;
		rangedPhalanxGeneric.range = 20;
		rangedPhalanxGeneric.rateOfFire = 1;
		rangedPhalanxGeneric.creditsDrop = 15;
		rangedPhalanxGeneric.level = 0;
		m_EnemyData[{rangedPhalanxGeneric.level, rangedPhalanxGeneric.name.c_str()}] = rangedPhalanxGeneric;
		variantData.list["RANGED_PHALANX_GENERIC"] = true;

		// Load base stats ranged phalanx red variant
		EnemyData rangedPhalanxRedVariant;
		rangedPhalanxRedVariant.name = "RANGED_PHALANX_REDVARIANT";
		rangedPhalanxRedVariant.health = 20;
		rangedPhalanxRedVariant.damage = 25;
		rangedPhalanxRedVariant.maxVelocity = 8;
		rangedPhalanxRedVariant.maxAcceleration = 8;
		rangedPhalanxRedVariant.stoppingDistance = 2;
		rangedPhalanxRedVariant.range = 20;
		rangedPhalanxRedVariant.rateOfFire = 1.5;
		rangedPhalanxRedVariant.creditsDrop = 20;
		rangedPhalanxRedVariant.level = 0;
		m_EnemyData[{rangedPhalanxRedVariant.level, rangedPhalanxRedVariant.name.c_str()}] = rangedPhalanxRedVariant;
		variantData.list["RANGED_PHALANX_REDVARIANT"] = true;

		// Load base stats ranged phalanx blue variant
		EnemyData rangedPhalanxblueVariant;
		rangedPhalanxblueVariant.name = "RANGED_PHALANX_BLUEVARIANT";
		rangedPhalanxblueVariant.health = 20;
		rangedPhalanxblueVariant.damage = 20;
		rangedPhalanxblueVariant.maxVelocity = 10;
		rangedPhalanxblueVariant.maxAcceleration = 10;
		rangedPhalanxblueVariant.stoppingDistance = 2;
		rangedPhalanxblueVariant.range = 20;
		rangedPhalanxblueVariant.rateOfFire = 3;
		rangedPhalanxblueVariant.creditsDrop = 25;
		rangedPhalanxblueVariant.level = 0;
		m_EnemyData[{rangedPhalanxblueVariant.level, rangedPhalanxblueVariant.name.c_str()}] = rangedPhalanxblueVariant;
		variantData.list["RANGED_PHALANX_BLUEVARIANT"] = true;

		// Load base stats sentinel
		EnemyData sentinel;
		sentinel.name = "SENTINEL";
		sentinel.health = 20;
		sentinel.damage = 25;
		sentinel.maxVelocity = 14;
		sentinel.maxAcceleration = 14;
		sentinel.stoppingDistance = 6;
		sentinel.range = 2;
		sentinel.rateOfFire = 0;
		sentinel.creditsDrop = 15;
		sentinel.level = 0;
		m_EnemyData[{sentinel.level, sentinel.name.c_str()}] = sentinel;

		// Load base stats subjugator
		EnemyData subjugator;
		subjugator.name = "SUBJUGATOR";
		subjugator.health = 40;
		subjugator.damage = 15;
		subjugator.maxVelocity = 12;
		subjugator.maxAcceleration = 12;
		subjugator.stoppingDistance = 2;
		subjugator.range = 40;
		subjugator.rateOfFire = 2;
		subjugator.creditsDrop = 30;
		subjugator.level = 0;
		m_EnemyData[{subjugator.level, subjugator.name.c_str()}] = subjugator;
		variantData.list["SUBJUGATOR"] = true;

		// Load base stats subjugatorChief
		EnemyData subjugatorChief;
		subjugatorChief.name = "SUBJUGATOR_CHIEF";
		subjugatorChief.health = 50;
		subjugatorChief.damage = 25;
		subjugatorChief.maxVelocity = 16;
		subjugatorChief.maxAcceleration = 16;
		subjugatorChief.stoppingDistance = 2;
		subjugatorChief.range = 40;
		subjugatorChief.rateOfFire = 2;
		subjugatorChief.creditsDrop = 35;
		subjugatorChief.level = 0;
		m_EnemyData[{subjugatorChief.level, subjugatorChief.name.c_str()}] = subjugatorChief;
		variantData.list["SUBJUGATOR_CHIEF"] = true;

		// Load base stats ultron
		EnemyData ultron;
		ultron.name = "ULTRON";
		ultron.health = 500;
		ultron.damage = 20;
		ultron.maxVelocity = 20;
		ultron.maxAcceleration = 12;
		ultron.stoppingDistance = 2;
		ultron.range = 15;
		ultron.rateOfFire = 2;
		ultron.creditsDrop = 35;
		ultron.level = 0;
		m_EnemyData[{ultron.level, ultron.name.c_str()}] = ultron;

		SpawnerData spawnerData;
		spawnerData.waveSpawnData.maxEnemiesPerWave = 10;
		spawnerData.waveSpawnData.minEnemiesPerWave = 5;
		spawnerData.waveSpawnData.enemySpawnRate = 1.5f;
		spawnerData.waveSpawnData.maxWaveCount = 3;
		spawnerData.waveSpawnData.timeBetweenWaves = 2.0f;
		spawnerData.waveSpawnData.waveChangeRate = 30.0f;
		// Index variants data
		for (int i = 0; i <= 10; i++)
		{
			variantData.n = i;
			spawnerData.n = i;
			m_VariantsTable.emplace_back(variantData);
			m_SpawnerDataTable.emplace_back(spawnerData);
		}

		json doc;
		doc["m_EnemyData"] = m_EnemyData;
		doc["m_MaxLevel"] = m_MaxLevel;
		doc["m_CurrentRunLevel"] = m_CurrentRunLevel;
		doc["m_IncreaseDiffEvery"] = m_IncreaseDiffEvery;
		doc["m_CurrentCombatRoomsCount"] = m_CurrentCombatRoomsCount;
		doc["m_CurrentRewardRoomCount"] = m_CurrentRewardRoomCount;

		doc["m_RangedData"]["bulletLifeTime"] = m_RangedData.bulletLifeTime;
		doc["m_RangedData"]["bulletSpeed"] = m_RangedData.bulletSpeed;

		doc["m_SubjugatorData"]["bulletLifeTime"] = m_SubjugatorData.bulletLifeTime;
		doc["m_SubjugatorData"]["bulletSpeed"] = m_SubjugatorData.bulletSpeed;

		doc["m_UltronData"]["bulletLifeTime"] = m_UltronData.bulletLifeTime;
		doc["m_UltronData"]["bulletSpeed"] = m_UltronData.bulletSpeed;

		doc["m_VariantsTable"] = m_VariantsTable;
		doc["m_SpawnerDataTable"] = m_SpawnerDataTable;

		std::ofstream file("config/enemy_data.json");
		if (file.is_open())
		{
			file << doc;
			file.close();
			WI_INFO("Enemy stats saved to enemy_stats.json");
			return true;
		}
		
		WI_INFO("Error: Could not open config/enemy_data.json for writing");
		return false;
	}

	void EnemyManager::Reset()
	{
		std::filesystem::path path;
		path = "config/enemy_data.json";
		std::filesystem::remove(path);

		CreateEmptyFile();
	}

	bool EnemyManager::CheckIfHasLevelCreate(int level, const std::string& str)
	{	
		std::pair<int, std::string> key = std::make_pair(level, str);
		EnemyData enemyData = m_EnemyData[{level - 1, str}];
		enemyData.level = level;

		// Check if the key exists in the map
		auto it = m_EnemyData.find(key);
		if (it == m_EnemyData.end()) {
			m_EnemyData.insert({ key, enemyData });
			return false;
		}
		return true;
	}

	bool EnemyManager::CheckIfHasLevelDelete(int level, const std::string& str)
	{
		std::pair<int, std::string> key = std::make_pair(level, str);
		EnemyData enemyData;
		enemyData.level = level;

		// Check if the key exists in the map
		auto it = m_EnemyData.find(key);
		if (it != m_EnemyData.end()) {
			m_EnemyData.erase(it);
			return false;
		}
		return true;
	}

	int EnemyManager::ResetDifficulty()
	{
		m_CurrentRunLevel = 0;
		m_CurrentRewardRoomCount = 0;
		return m_CurrentRunLevel;
	}

	int EnemyManager::IncreaseDifficulty()
	{
		m_CurrentRunLevel++;
		if (m_CurrentRunLevel >= m_MaxLevel)
			m_CurrentRunLevel = m_MaxLevel;
		return m_CurrentRunLevel;
	}

	int EnemyManager::DecreaseDifficulty()
	{
		m_CurrentRunLevel--;
		if (m_CurrentRunLevel <= 0)
			m_CurrentRunLevel = 0;
		return m_CurrentRunLevel;
	}

	int EnemyManager::IncreaseRoomRewardCounter()
	{
		m_CurrentRewardRoomCount++;
		if (m_CurrentRewardRoomCount >= m_IncreaseDiffEvery)
		{
			IncreaseDifficulty();
			m_CurrentRewardRoomCount = 0;
		}
		return m_CurrentRunLevel;
	}
}
