//#include "AchievementsPanel.h"
//#include "../../Utils/EditorUtils.h"
//#include <Wiwa/game/GameStateManager.h>
//#include <Wiwa/game/Achievements/Property.h>
//#include <Wiwa/game/Achievements/Achievement.h>
//#include <Wiwa/game/Achievements/AchievementsManager.h>
//#include <map>
//#include <cstdlib> 
//#define MAX_DESCRIPTION_CHARACTERS 256
//
//AchievementsPanel::AchievementsPanel(EditorLayer* instance)
//	: Panel("Achievements", ICON_FK_LIST_UL, instance)
//{
//
//}
//
//AchievementsPanel::~AchievementsPanel()
//{
//}
//
//void AchievementsPanel::Draw()
//{
//	ImGui::Begin(iconName.c_str(), &active);
//
//	if (ImGui::CollapsingHeader("Properties"))
//	{
//        ImGui::PushID("Property");
//        // Edit name
//        if (ImGui::InputText("Name", &m_Name, 56)) {
//            // Update map with new name
//           
//        }
//
//        // Edit description
//        if (ImGui::InputText("Description", &m_Description)) {
//        }
//
//        // Edit activation rule
//        int activationIndex = m_Activation;
//        if (ImGui::Combo("Activation Rule", &activationIndex, "None\0GreaterThan\0LessThan\0EqualTo\0")) {
//            m_Activation = activationIndex;
//        }
//
//        // Edit activation value
//        if (m_Activation != (int)Wiwa::ActivationRules::NONE) {
//            if (ImGui::InputInt("Activation Value", &m_ActivationValue)) {
//              
//            }
//        }
//
//        // Edit initial value
//        if (ImGui::InputInt("Initial Value", &m_InitialValue)) {   
//        }
//
//        // Edit value
//        if (ImGui::InputInt("Value", &m_Value)) {
//           
//        }
//
//        ImGui::Separator();
//  
//      
//        // Button to add new property
//        if (ImGui::Button("Add Property")) {   
//            Wiwa::AchievementsManager::DefineProperty(m_Name,m_Description,m_InitialValue,(Wiwa::ActivationRules)m_Activation,m_Value);
//        }
//
//        ShowPropertiesTable(Wiwa::AchievementsManager::GetProperties());
//       
//		ImGui::Separator();
//		if (ImGui::Button("Save"))
//		{
//			Wiwa::GameStateManager::SerializeData();
//		}
//
//        ImGui::PopID();
//	}
//	if (ImGui::CollapsingHeader("Achievements"))
//	{
//        ImGui::PushID("Achievement");
//        // Edit name
//        if (ImGui::InputText("Name", &m_Name_Achievement, 56)) {
//        }
//
//        // Edit description
//        if (ImGui::InputText("Description", &m_Description_Achievement)) {
//        }
//
//        PropertyMapAchievements();
//        
//        if (ImGui::Checkbox("Unlocked", &m_Unlocked_Achievement)) {
//        }
//
//       
//        m_NumTier_Achievement = achievement_properties_amount;
//        
//
//        if (ImGui::InputInt("Current tier achievement", &m_Tier_Achievement)) {
//        }
//
//        if (ImGui::InputInt("Current cost achievement", &m_CurrentCost_Achievement)) {
//        }
//        ImGui::Separator();
//        ImGui::PushID("CostsVector");
//        ImGui::Text("Costs:");
//        VectorEdit(m_Costs_Achievement);
//        ImGui::PopID();
//        ImGui::Separator();
//
//		if (ImGui::Button("Add achievement"))
//		{
//            Wiwa::AchievementsManager::DefineAchievement(m_Name_Achievement, m_Description_Achievement, m_NumTier_Achievement, m_Costs_Achievement, m_Properties_Achievement);
//		}
//         
//        ShowAchievementsTable(Wiwa::AchievementsManager::GetAchievements());
//
//
//		ImGui::Separator();
//		if (ImGui::Button("Save"))
//		{
//			Wiwa::GameStateManager::SerializeData();
//		}
//        ImGui::PopID();
//	}
//	ImGui::End();
//}
//
//void AchievementsPanel::OnEvent(Wiwa::Event& e)
//{
//}
//
//// Assuming ImGui is properly initialized
//void AchievementsPanel::AchievementsPanel::ShowPropertiesTable(const std::map<std::string, Wiwa::Property>& properties)
//{
//    // Table header
//    ImGui::Text("Name");
//    ImGui::SameLine(150);
//    ImGui::Text("Description");
//    ImGui::SameLine(400);
//    ImGui::Text("Value");
//
//    // Separator line
//    ImGui::Separator();
//
//    // Table rows
//    for (const auto& [name, property] : properties)
//    {
//        ImGui::Text(name.c_str());
//        ImGui::SameLine(150);
//        ImGui::Text(property.m_Description.c_str());
//        ImGui::SameLine(400);
//
//        // Edit property value
//        int value = property.m_Value;
//        if (ImGui::InputInt("##value", &value))
//        {
//            // Update property value
//            const_cast<Wiwa::Property&>(property).m_Value = value;
//        }
//    }
//}
//
//void AchievementsPanel::ShowAchievementsTable(const std::map<std::string, Wiwa::Achievement>& achievement)
//{
//    // Table header
//    ImGui::Text("Name");
//    ImGui::SameLine(150);
//    ImGui::Text("Description");
//    ImGui::SameLine(400);
//    ImGui::Text("Value");
//
//    // Separator line
//    ImGui::Separator();
//
//    // Table rows
//    for (const auto& [name, achievement] : achievement)
//    {
//        ImGui::Text(name.c_str());
//        ImGui::SameLine(150);
//        ImGui::Text(achievement.m_Description.c_str());
//        ImGui::SameLine(400);
//
//        // Edit property value
//        int tier = achievement.m_Tier;
//        if (ImGui::InputInt("##tier", &tier))
//        {
//            // Update property value
//            const_cast<Wiwa::Achievement&>(achievement).m_Tier = tier;
//        }
//    }
//}
//
//void AchievementsPanel::PropertyMapAchievements()
//{
//    std::map<std::string, Wiwa::Property> mapProperties = Wiwa::AchievementsManager::GetProperties();
//    std::vector<std::string> mapKeys;
//    int itemSelected = 0;
//    for (auto const& element : mapProperties) {
//        mapKeys.push_back(element.first);
//    }
//
//    for (size_t i = 0; i < achievement_properties_amount; i++)
//    {
//        ImGui::PushID(i);
//        const char** comboContents = new const char* [mapKeys.size()];
//        for (int i = 0; i < mapKeys.size(); i++) {
//            comboContents[i] = mapKeys[i].c_str();
//        }
//
//        int activationIndex = m_PropertyIterator[i];
//        if (ImGui::Combo("Property", &activationIndex, comboContents, mapKeys.size())) {
//            m_PropertyIterator[i] = activationIndex;
//        }
//        ImGui::PopID();
//    }
//    if (ImGui::Button("+"))
//    {
//        if(achievement_properties_amount < MAX_PROPERTIES_PER_ACHIEVEMENT)
//            achievement_properties_amount++;
//    }
//    ImGui::SameLine();
//    if (ImGui::Button("-"))
//    {
//        if (achievement_properties_amount > 0)
//        {
//            achievement_properties_amount--;
//            if (m_Properties_Achievement.size() > 0)m_Properties_Achievement.erase(m_Properties_Achievement.begin() + m_Properties_Achievement.size() - 1);
//        }
//    }
//    ImGui::SameLine();
//    if (ImGui::Button("Add properties"))
//    {
//        for (int i = 0; i < achievement_properties_amount; i++)
//        {
//            m_Properties_Achievement.push_back(*Wiwa::AchievementsManager::GetProperty(mapKeys[m_PropertyIterator[i]].c_str()));
//
//        }
//    }
//   
//}
