#pragma once

#include "Panel.h"

class InventoryPanel : public Panel
{
public:
	InventoryPanel(EditorLayer* instance);
	virtual ~InventoryPanel();

	void Draw() override;
	void DrawConsumablePool(int& id);
	void DrawBuffPool(int& id);
	void DrawPassivePool(int& id);
	void DrawAbilityPool(int& id);
	void DrawShopElementPool(int& id);
};