//#pragma once
//#include "Panel.h"
//#include <map>
//#include <string>
//
//#include <Wiwa/game/Achievements/Property.h>
//#include <Wiwa/game/Achievements/Achievement.h>
//
//#define MAX_PROPERTIES_PER_ACHIEVEMENT 5
//class AchievementsPanel : public Panel
//{
//public:
//	AchievementsPanel(EditorLayer* instance);
//	virtual ~AchievementsPanel();
//
//	void Draw() override;
//
//	void OnEvent(Wiwa::Event& e) override;
//	void ShowPropertiesTable(const std::map < std::string, Wiwa::Property > & properties);
//	void ShowAchievementsTable(const std::map < std::string, Wiwa::Achievement >& achievement);
//
//	void PropertyMapAchievements();
//private:
//	//PROPERTIES VALUE
//	std::string m_Name = "";
//	std::string m_Description = "";
//	int m_Activation = 0;
//	int m_Value = 0;
//	int m_ActivationValue = 0;
//	int m_InitialValue = 0;
//
//	//ACHIEVEMENTS VALUE
//	int achievement_properties_amount = 0;
//
//	std::string m_Name_Achievement = "";
//	std::string m_Description_Achievement = "";
//	int m_PropertyIterator [MAX_PROPERTIES_PER_ACHIEVEMENT] = {0,0,0,0,0};
//	std::vector<Wiwa::Property> m_Properties_Achievement;
//	bool m_Unlocked_Achievement = false;
//	int m_NumTier_Achievement = 0;
//	int m_Tier_Achievement = 0;
//	int m_CurrentCost_Achievement = 0;
//	std::vector<int> m_Costs_Achievement;
//};
//
