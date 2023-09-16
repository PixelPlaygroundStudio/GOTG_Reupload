#pragma once
#include <Wiwa/core/Core.h>
#include <Wiwa/ecs/Systems.h>
#include <Wiwa/ecs/components/game/items/Item.h>
#include <Wiwa/ecs/components/game/Character.h>
#include "../CharacterInventory.h"

namespace Wiwa
{
    class WI_API PlayerGUISystem : public System
    {
    private:
        int CurrentHUD = 0;
        int CanvasHUD = 0;
        int PauseHUD = 1;
        int OptionsHUD = 2;
        int DeathHUD = 3;
        int ShopHUD = 4;

        bool deathHud = false;
        bool returnToHUD = false;
        bool returnToPauseHUD = false;
        bool pauseGame = false;
        bool shopHUD = false;
        bool updateShopHUD = false;

        bool shopActive = false;
        bool buyItem = false;
        int CurrentWave = 0;
        size_t lastCoins = -1;
        size_t lastPromethiumGems = -1;
        Ability** abilitiesList;
        Buff** buffsList;
        std::vector<PassiveSkill> passives;

        float m_CurrentTime;
        float m_Frequency;
    public:
        PlayerGUISystem() = default;
        virtual ~PlayerGUISystem() = default;

        void OnInit() override;
        void OnUpdate() override;

        void HandleCurrentCanvas(Wiwa::GuiManager& gm);

        void DeathHud(Wiwa::GuiManager& gm);

        void CooldownState(Ability** ability,Wiwa::GuiManager& gm);
        void CooldownState(Buff** buff, Wiwa::GuiManager& gm);

        void HandleActiveAbilities(Ability** ability, Wiwa::GuiManager& gm);
        void HandleActiveBuffs(Buff** buff, Wiwa::GuiManager& gm);
        void HandleActivePassives(std::vector<PassiveSkill> PassiveSkills, Wiwa::GuiManager& gm);
        void PlayerElements(Wiwa::GuiManager& gm, Character* character);

        void Coins(uint32_t coins, Wiwa::GuiManager& gm);
        void PromethiumGems(uint32_t promethiumGems, Wiwa::GuiManager& gm);

        void HandlePlayerStatistics(int max_Health, int max_Shield, float range, int attack, float speed, float rateOfFire, Wiwa::GuiManager& gm);

        void ShopElementsHUD(Item* currentItem , Wiwa::GuiManager& gm);

        void OnCollisionEnter(Object* body1, Object* body2) override;

        void OnCollisionExit(Object* body1, Object* body2) override;

        void HandleWaves(Wiwa::GuiManager& gm);

        void EnableShopUI();
        void DisableShopUI();

    };
}

REGISTER_SYSTEM(Wiwa::PlayerGUISystem);