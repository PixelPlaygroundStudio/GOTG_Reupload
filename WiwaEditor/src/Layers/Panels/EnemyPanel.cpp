#include "EnemyPanel.h"
#include <Wiwa/game/GameStateManager.h>

static ImGuiTableFlags s_FlagsEnemyTable = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_BordersInnerV
| ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_Resizable;

EnemyPanel::EnemyPanel(EditorLayer* instance)
	: Panel("Enemy Panel", ICON_FK_MAGIC, instance)
{
	Init();
}

EnemyPanel::~EnemyPanel()
{
}

void EnemyPanel::Init()
{
	Wiwa::GameStateManager::GetEnemyManager().DeSerialize();
}

void EnemyPanel::Draw()
{
	ImGui::Begin(iconName.c_str(), &active);

	DrawTables();

	DrawInputs();

	ImGui::Text("Hard CleanUp: Resets everythin to base level");
	ImGui::SameLine();
	if (ImGui::Button("Reset"))
		Wiwa::GameStateManager::GetEnemyManager().Reset();

	ImGui::SameLine();
	if (ImGui::Button("Load"))
		Wiwa::GameStateManager::GetEnemyManager().DeSerialize();

	ImGui::Text("Remember to save");
	ImGui::SameLine();
	if (ImGui::Button("Save"))
		Wiwa::GameStateManager::GetEnemyManager().Serialize();

	ImGui::End();
}

void EnemyPanel::DrawTables()
{
	Wiwa::EnemyManager& manager = Wiwa::GameStateManager::GetEnemyManager();
	std::map<std::pair<int, std::string>, Wiwa::EnemyData>& data = manager.GetData();

	if (ImGui::TreeNode("Enemies stats"))
	{
		DrawEnemyStatsTable("Melee Phalanx Generic", "MELEE_PHALANX_GENERIC");
		DrawEnemyStatsTable("Melee Phalanx Red Variant", "MELEE_PHALANX_REDVARIANT");
		DrawEnemyStatsTable("Melee Phalanx Blue Variant", "MELEE_PHALANX_BLUEVARIANT");

		DrawEnemyStatsTable("Ranged Phalanx Generic", "RANGED_PHALANX_GENERIC");
		DrawEnemyStatsTable("Ranged Phalanx Red Variant", "RANGED_PHALANX_REDVARIANT");
		DrawEnemyStatsTable("Ranged Phalanx Blue Variant", "RANGED_PHALANX_BLUEVARIANT");

		DrawEnemyStatsTable("Sentinel", "SENTINEL");

		DrawEnemyStatsTable("Subjugator", "SUBJUGATOR");
		DrawEnemyStatsTable("Subjugator Chief", "SUBJUGATOR_CHIEF");

		DrawEnemyStatsTable("Ultron", "ULTRON");
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Enemies bullets & attacks"))
	{
		DrawRangedData();
		DrawSubjugatorData();
		DrawUltronData();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Spawn data"))
	{
		DrawVariantsTable();
		ImGui::TreePop();
	}
}

void EnemyPanel::DrawEnemyStatsTable(const char* label, const char* enemy_str)
{
	Wiwa::EnemyManager& manager = Wiwa::GameStateManager::GetEnemyManager();
	std::map<std::pair<int, std::string>, Wiwa::EnemyData>& data = manager.GetData();

	if (ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginTable(enemy_str, 9, s_FlagsEnemyTable))
		{
			ImGui::TableSetupColumn("Level");
			ImGui::TableSetupColumn("Health");
			ImGui::TableSetupColumn("Damage");
			ImGui::TableSetupColumn("MaxVelocity");
			ImGui::TableSetupColumn("MaxAcceleration");
			ImGui::TableSetupColumn("StoppingDistance");
			ImGui::TableSetupColumn("RateOfFire");
			ImGui::TableSetupColumn("Range");
			ImGui::TableSetupColumn("CreditsDrop");

			ImGui::TableHeadersRow();

			for (int i = 0; i <= manager.m_MaxLevel; i++)
			{
				manager.CheckIfHasLevelCreate(i, enemy_str);
				Wiwa::EnemyData& value = data.at({ i, enemy_str });

				ImGui::TableNextRow();
				// Name
			/*	ImGui::TableSetColumnIndex(0);
				ImGui::TextUnformatted(value.name.c_str());*/

				// Level
				ImGui::TableSetColumnIndex(0);
				ImGui::TextUnformatted(std::to_string(value.level).c_str());

				// Health
				ImGui::TableSetColumnIndex(1);
				ImGui::InputInt(("##health" + std::to_string(i)).c_str(), &value.health);

				// Damage
				ImGui::TableSetColumnIndex(2);
				ImGui::InputInt(("##damage" + std::to_string(i)).c_str(), &value.damage);

				// Max Velocity
				ImGui::TableSetColumnIndex(3);
				ImGui::InputFloat(("##maxVelocity" + std::to_string(i)).c_str(), &value.maxVelocity, 0.0f, 0.0f, "%.1f");

				// Max Acceleration
				ImGui::TableSetColumnIndex(4);
				ImGui::InputFloat(("##maxAcceleration" + std::to_string(i)).c_str(), &value.maxAcceleration, 0.0f, 0.0f, "%.1f");

				// Stopping distance
				ImGui::TableSetColumnIndex(5);
				ImGui::InputFloat(("##stoppingDistance" + std::to_string(i)).c_str(), &value.stoppingDistance, 0.0f, 0.0f, "%.1f");

				// Rate of fire
				ImGui::TableSetColumnIndex(6);
				ImGui::InputFloat(("##rateOfFire" + std::to_string(i)).c_str(), &value.rateOfFire, 0.0f, 0.0f, "%.1f");

				// Range
				ImGui::TableSetColumnIndex(7);
				ImGui::InputFloat(("##range" + std::to_string(i)).c_str(), &value.range, 0.0f, 0.0f, "%.1f");

				// Credits drop
				ImGui::TableSetColumnIndex(8);
				ImGui::InputInt(("##creditsDrop" + std::to_string(i)).c_str(), &value.creditsDrop);
			}
			ImGui::EndTable();
		}
		ImGui::TreePop();
	}
}

void EnemyPanel::DrawInputs()
{
	Wiwa::EnemyManager& manager = Wiwa::GameStateManager::GetEnemyManager();
	int previousMaxLevel = manager.m_MaxLevel;

	ImGui::InputInt("Maximum level of difficulty", &manager.m_MaxLevel);
	ImGui::InputInt("Level of difficulty in the current run", &manager.m_CurrentRunLevel);
	ImGui::InputInt("Increase difficulty ever <<n>> reward rooms", &manager.m_IncreaseDiffEvery);

	if (manager.m_MaxLevel < previousMaxLevel)
	{
		manager.CheckIfHasLevelDelete(previousMaxLevel, "MELEE_PHALANX_GENERIC");
		manager.CheckIfHasLevelDelete(previousMaxLevel, "MELEE_PHALANX_REDVARIANT");
		manager.CheckIfHasLevelDelete(previousMaxLevel, "MELEE_PHALANX_BLUEVARIANT");

		manager.CheckIfHasLevelDelete(previousMaxLevel, "RANGED_PHALANX_GENERIC");
		manager.CheckIfHasLevelDelete(previousMaxLevel, "RANGED_PHALANX_REDVARIANT");
		manager.CheckIfHasLevelDelete(previousMaxLevel, "RANGED_PHALANX_BLUEVARIANT");

		manager.CheckIfHasLevelDelete(previousMaxLevel, "SENTINEL");
		manager.CheckIfHasLevelDelete(previousMaxLevel, "SUBJUGATOR");
		manager.CheckIfHasLevelDelete(previousMaxLevel, "SUBJUGATOR_CHIEF");
		manager.CheckIfHasLevelDelete(previousMaxLevel, "ULTRON");
	}

	if (manager.m_MaxLevel < 0)
		manager.m_MaxLevel = 0;

	if (manager.m_CurrentRunLevel < 0)
		manager.m_CurrentRunLevel = 0;

	if (manager.m_IncreaseDiffEvery < 0)
		manager.m_IncreaseDiffEvery = 0;
}

void EnemyPanel::DrawVariantsTable()
{
	Wiwa::EnemyManager& manager = Wiwa::GameStateManager::GetEnemyManager();
	std::vector<Wiwa::VariantData>& data1 = manager.m_VariantsTable;
	std::vector<Wiwa::SpawnerData>& data2 = manager.m_SpawnerDataTable;

	if (ImGui::TreeNodeEx("Spawner & Variants data (At percentage of the run defined by the number of combat rooms)", ImGuiTreeNodeFlags_DefaultOpen)){
		if (ImGui::BeginTable("Variants table", 11, s_FlagsEnemyTable)){

			ImGui::TableSetupColumn("0%");
			ImGui::TableSetupColumn("10%");
			ImGui::TableSetupColumn("20%");
			ImGui::TableSetupColumn("30%");
			ImGui::TableSetupColumn("40%");
			ImGui::TableSetupColumn("50%");
			ImGui::TableSetupColumn("60%");
			ImGui::TableSetupColumn("70%");
			ImGui::TableSetupColumn("80%");
			ImGui::TableSetupColumn("90%");
			ImGui::TableSetupColumn("100%");

			ImGui::TableHeadersRow();
			ImGui::TableNextRow();

			for (int i = 0; i <= 10; i++){
				ImGui::TableSetColumnIndex(i);
				for (auto& [key, value] : data1[i].list){
					if (i == 0)
						ImGui::Checkbox((key + std::to_string(i)).c_str(), &value);
					else
						ImGui::Checkbox(("##" + key + std::to_string(i)).c_str(), &value);
				}

				if (i == 0)
				{
					ImGui::Separator();
					Wiwa::SpawnerData& s = data2[i];
					ImGui::InputInt(("maxEnemiesPerWave" + std::to_string(i)).c_str(), &s.waveSpawnData.maxEnemiesPerWave);
					ImGui::InputInt(("minEnemiesPerWave" + std::to_string(i)).c_str(), &s.waveSpawnData.minEnemiesPerWave);
					ImGui::InputFloat(("enemySpawnRate" + std::to_string(i)).c_str(), &s.waveSpawnData.enemySpawnRate, 0.0f, 0.0f, "%.1f");
					ImGui::InputInt(("maxWaveCount" + std::to_string(i)).c_str(), &s.waveSpawnData.maxWaveCount);
					ImGui::InputFloat(("timeBetweenWaves" + std::to_string(i)).c_str(), &s.waveSpawnData.timeBetweenWaves, 0.0f, 0.0f, "%.1f");
					ImGui::InputFloat(("waveChangeRate" + std::to_string(i)).c_str(), &s.waveSpawnData.waveChangeRate, 0.0f, 0.0f, "%.1f");
				}
				else
				{
					ImGui::Separator();
					Wiwa::SpawnerData& s = data2[i];
					ImGui::InputInt(("##maxEnemiesPerWave" + std::to_string(i)).c_str(), &s.waveSpawnData.maxEnemiesPerWave);
					ImGui::InputInt(("##minEnemiesPerWave" + std::to_string(i)).c_str(), &s.waveSpawnData.minEnemiesPerWave);
					ImGui::InputFloat(("##enemySpawnRate" + std::to_string(i)).c_str(), &s.waveSpawnData.enemySpawnRate, 0.0f, 0.0f, "%.1f");
					ImGui::InputInt(("##maxWaveCount" + std::to_string(i)).c_str(), &s.waveSpawnData.maxWaveCount);
					ImGui::InputFloat(("##timeBetweenWaves" + std::to_string(i)).c_str(), &s.waveSpawnData.timeBetweenWaves, 0.0f, 0.0f, "%.1f");
					ImGui::InputFloat(("##waveChangeRate" + std::to_string(i)).c_str(), &s.waveSpawnData.waveChangeRate, 0.0f, 0.0f, "%.1f");
				}
				
			}
			ImGui::EndTable();
		}
		ImGui::TreePop();
	}
}

void EnemyPanel::DrawRangedData()
{
	Wiwa::EnemyManager& manager = Wiwa::GameStateManager::GetEnemyManager();
	if (ImGui::TreeNodeEx("Ranged Enemy Behavior Data", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::InputFloat("bulletSpeedR", &manager.m_RangedData.bulletSpeed, 0.0f, 0.0f, "%.1f");
		ImGui::InputFloat("bulletLifetimeR", &manager.m_RangedData.bulletLifeTime, 0.0f, 0.0f, "%.1f");
		ImGui::TreePop();
	}
}

void EnemyPanel::DrawSubjugatorData()
{
	Wiwa::EnemyManager& manager = Wiwa::GameStateManager::GetEnemyManager();
	if (ImGui::TreeNodeEx("Subjugator Enemy Behavior Data", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::InputFloat("bulletSpeedS", &manager.m_SubjugatorData.bulletSpeed, 0.0f, 0.0f, "%.1f");
		ImGui::InputFloat("bulletLifetimeS", &manager.m_SubjugatorData.bulletLifeTime, 0.0f, 0.0f, "%.1f");
		ImGui::TreePop();
	}
}

void EnemyPanel::DrawUltronData()
{
	Wiwa::EnemyManager& manager = Wiwa::GameStateManager::GetEnemyManager();
	if (ImGui::TreeNodeEx("Ultron Enemy Behavior Data", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::InputFloat("bulletSpeedU", &manager.m_UltronData.bulletSpeed, 0.0f, 0.0f, "%.1f");
		ImGui::InputFloat("bulletLifetimeU", &manager.m_UltronData.bulletLifeTime, 0.0f, 0.0f, "%.1f");

		// Splash
		ImGui::InputInt("bulletStorm_firstPattern_bulletsPerWave", &manager.m_UltronData.bulletStorm_firstPattern_bulletsPerWave);
		ImGui::InputInt("bulletStorm_firstPattern_waves", &manager.m_UltronData.bulletStorm_firstPattern_waves);

		// Spiral
		ImGui::InputInt("bulletStorm_secondPattern_bulletsPerSpiral", &manager.m_UltronData.bulletStorm_secondPattern_bulletsPerSpiral);
		ImGui::InputFloat("bulletStorm_secondPattern_timerBetweenSpirals", &manager.m_UltronData.bulletStorm_secondPattern_timerBetweenSpirals, 0.0f, 0.0f, "%.1f");
		ImGui::InputInt("bulletStorm_secondPattern_spirals", &manager.m_UltronData.bulletStorm_secondPattern_spirals);

		// Cross
		ImGui::InputInt("bulletStorm_thirdPattern_bulletsPerCross", &manager.m_UltronData.bulletStorm_thirdPattern_bulletsPerCross);
		ImGui::InputFloat("bulletStorm_thirdPattern_timerBetweenCross", &manager.m_UltronData.bulletStorm_thirdPattern_timerBetweenCross, 0.0f, 0.0f, "%.1f");
		ImGui::InputInt("bulletStorm_thirdPattern_crosses", &manager.m_UltronData.bulletStorm_thirdPattern_crosses);

		/*-------- Cluster Shot --------*/
		ImGui::InputInt("clusterShot_bullets", &manager.m_UltronData.clusterShot_bullets);
		ImGui::InputFloat("clusterShot_timerBetweenBullets", &manager.m_UltronData.clusterShot_timerBetweenBullets, 0.0f, 0.0f, "%.1f");
		ImGui::InputInt("clusterShot_miniBullets", &manager.m_UltronData.clusterShot_miniBullets);

		/*-------- Laser beam --------*/
		ImGui::InputInt("laserBeam_tickDamage", &manager.m_UltronData.laserBeam_tickDamage);
		ImGui::InputFloat("laserBeam_tickTimer", &manager.m_UltronData.laserBeam_tickTimer, 0.0f, 0.0f, "%.1f");
		ImGui::InputInt("laserBeam_duration", &manager.m_UltronData.laserBeam_duration);

		ImGui::TreePop();
	}
}

	//if (ImGui::BeginTable("Enemies stats", 9))
	//{
	//	ImGui::TableSetupColumn("Name");
	//	ImGui::TableSetupColumn("Level");
	//	ImGui::TableSetupColumn("Health");
	//	ImGui::TableSetupColumn("Damage");
	//	ImGui::TableSetupColumn("MaxVelocity");
	//	ImGui::TableSetupColumn("MaxAcceleration");
	//	ImGui::TableSetupColumn("RateOfFire");
	//	ImGui::TableSetupColumn("Range");
	//	ImGui::TableSetupColumn("CreditsDrop");
	//	
	//	for (const auto& [key, value] : data)
	//	{
	//		ImGui::TableNextRow();
	//		// Name
	//		ImGui::TableSetColumnIndex(0);
	//		ImGui::TextUnformatted(value.name.c_str());

	//		// Level
	//		ImGui::TableSetColumnIndex(1);
	//		ImGui::TextUnformatted(std::to_string(value.level).c_str());

	//		// Health
	//		ImGui::TableSetColumnIndex(2);
	//		ImGui::TextUnformatted(std::to_string(value.health).c_str());

	//		// Damage
	//		ImGui::TableSetColumnIndex(3);
	//		ImGui::TextUnformatted(std::to_string(value.damage).c_str());

	//		// Max Velocity
	//		ImGui::TableSetColumnIndex(4);
	//		ImGui::TextUnformatted(std::to_string(value.maxVelocity).c_str());

	//		// Max Acceleration
	//		ImGui::TableSetColumnIndex(5);
	//		ImGui::TextUnformatted(std::to_string(value.maxAcceleration).c_str());

	//		// Rate of fire
	//		ImGui::TableSetColumnIndex(6);
	//		ImGui::TextUnformatted(std::to_string(value.rateOfFire).c_str());

	//		// Range
	//		ImGui::TableSetColumnIndex(7);
	//		ImGui::TextUnformatted(std::to_string(value.range).c_str());

	//		// Credits drop
	//		ImGui::TableSetColumnIndex(8);
	//		ImGui::TextUnformatted(std::to_string(value.creditsDrop).c_str());
	//	}
	//	ImGui::EndTable();
	//}
	

