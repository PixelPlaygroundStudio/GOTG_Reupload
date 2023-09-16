#pragma once
#include <Wiwa/core/Core.h>
#include <Wiwa/ecs/Systems.h>
#include <Wiwa/ecs/components/game/Character.h>

namespace Wiwa
{
    // This system only takes care of the pickup system
    struct Item;
    class WI_API CharacterInventory : public System
    {
    private:
        bool buyItem = false;
        bool shopActive = false;
        bool releasingShop = false;
        Character lastCharacter;
        Item* currentItem;
        EntityId currentItemShopId;
    public:
        CharacterInventory() = default;
        virtual ~CharacterInventory() = default;

        void OnInit() override;

        void OnUpdate() override;

        bool ShopElement(Item* item);

        void SetCurrentItem(Item* item, EntityId goID);

        void OnCollisionEnter(Object* body1, Object* body2) override;
        void OnCollisionExit(Object* body1, Object* body2) override;

        void SetShopActive(bool active);

        bool GetShopActive() { return shopActive; }

        Item* GetCurrentShopItem() { return currentItem; }

        EntityId GetShopItemId() { return currentItemShopId; }
        bool PlayerUpdated();
    };
}
REGISTER_SYSTEM(Wiwa::CharacterInventory);