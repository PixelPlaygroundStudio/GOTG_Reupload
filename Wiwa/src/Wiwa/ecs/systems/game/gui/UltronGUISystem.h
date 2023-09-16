#pragma once
#include <Wiwa/core/Core.h>
#include <Wiwa/ecs/Systems.h>
#include <Wiwa/ecs/components/game/items/Item.h>
#include <Wiwa/ecs/components/game/Character.h>
#include "../CharacterInventory.h"

namespace Wiwa
{
    class WI_API UltronGUISystem : public System
    {
    private:
        bool bossIsDead = false;
        bool activeBossCanvas = false;
        bool introBossFight = true;
        bool activeIntroCanvas = false;
        bool m_CanvasHUD = true;
        float counterIntroAnim = 0.0f;
        float timeIntroAnim = 100.0f;
        bool m_IterationForPause = false;

        float maxUltronHealth = 0.0f;
    public:
        UltronGUISystem() = default;
        virtual ~UltronGUISystem() = default;

        void OnUpdate() override;

        bool AnimationIntroBoss(Wiwa::GuiManager& gm);

        void OnCollisionEnter(Object* body1, Object* body2) override;

        void UpdateBattleName(std::string name,Wiwa::GuiManager& gm);
    };
}

REGISTER_SYSTEM(Wiwa::UltronGUISystem);