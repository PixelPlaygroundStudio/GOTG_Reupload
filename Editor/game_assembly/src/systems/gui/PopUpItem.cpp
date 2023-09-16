#include "PopUpItem.h"
#include <Wiwa/game/GameStateManager.h>
#include <Wiwa/core/Input.h>
#include <Wiwa/Ui/UiManager.h>
#include <Wiwa/game/Items/ItemManager.h>


namespace Wiwa
{

	Wiwa::PopUpItem::PopUpItem()
	{

	}

	PopUpItem::~PopUpItem()
	{
	}

	void PopUpItem::OnAwake()
	{
	}

	void PopUpItem::OnInit()
	{
		m_Animate = false;
		m_PopUp = false;
		m_PopOut = false;
		m_TimerIn = 0.0f;
		m_TimerOut = 0.0f;
		m_TimerAnim = 255.0f;
	}

	void PopUpItem::OnUpdate()
	{
		Wiwa::GuiManager& gm = m_Scene->GetGuiManager();
		if (m_Animate)
		{
			if (m_PopUp)
			{
				m_TimerIn++;
				gm.canvas.at(11)->controls.at(0)->MoveGUIElement({ 451.0f,900.0f }, m_TimerAnim, {(float)gm.canvas.at(11)->controls.at(0)->position.x,(float)gm.canvas.at(11)->controls.at(0)->position.y}, GuiControlEasing::EaseBounceIn);
				gm.canvas.at(11)->controls.at(1)->MoveGUIElement({ 856.0f,1000.0f }, m_TimerAnim, { (float)gm.canvas.at(11)->controls.at(1)->position.x,(float)gm.canvas.at(11)->controls.at(1)->position.y }, GuiControlEasing::EaseBounceIn);
				gm.canvas.at(11)->controls.at(2)->MoveGUIElement({ 821.0f,1275.0f }, m_TimerAnim, { (float)gm.canvas.at(11)->controls.at(2)->position.x,(float)gm.canvas.at(11)->controls.at(2)->position.y }, GuiControlEasing::EaseBounceIn);
				gm.canvas.at(11)->controls.at(3)->MoveGUIElement({ 821.0f,1375.0f }, m_TimerAnim, { (float)gm.canvas.at(11)->controls.at(2)->position.x,(float)gm.canvas.at(11)->controls.at(2)->position.y }, GuiControlEasing::EaseBounceIn);

				if (m_TimerIn >= 600)
				{
					m_PopUp = false;
					m_PopOut = true;
				}
			}
			if (m_PopOut)
			{
				m_TimerOut++;
				gm.canvas.at(11)->controls.at(0)->MoveGUIElement({ 312.0f,2000.0f }, m_TimerAnim, { (float)gm.canvas.at(11)->controls.at(0)->position.x,(float)gm.canvas.at(11)->controls.at(0)->position.y }, GuiControlEasing::EaseBounceIn);
				gm.canvas.at(11)->controls.at(1)->MoveGUIElement({ 600.0f,2000.0f }, m_TimerAnim, { (float)gm.canvas.at(11)->controls.at(1)->position.x,(float)gm.canvas.at(11)->controls.at(1)->position.y }, GuiControlEasing::EaseBounceIn);
				gm.canvas.at(11)->controls.at(2)->MoveGUIElement({ 615.0f,2000.0f }, m_TimerAnim, { (float)gm.canvas.at(11)->controls.at(2)->position.x,(float)gm.canvas.at(11)->controls.at(2)->position.y }, GuiControlEasing::EaseBounceIn);
				gm.canvas.at(11)->controls.at(3)->MoveGUIElement({ 615.0f,2000.0f }, m_TimerAnim, { (float)gm.canvas.at(11)->controls.at(2)->position.x,(float)gm.canvas.at(11)->controls.at(2)->position.y }, GuiControlEasing::EaseBounceIn);

				if (m_TimerOut >= 600)
				{
					m_PopOut = false;
				}
			}

			if (!m_PopOut && !m_PopUp)
			{
				gm.canvas.at(11)->controls.at(0)->position.y = -400.0f;
				gm.canvas.at(11)->controls.at(1)->position.y = -400.0f;
				gm.canvas.at(11)->controls.at(2)->position.y = -400.0f;
				gm.canvas.at(11)->controls.at(3)->position.y = -400.0f;

			}
		}
	}

	void PopUpItem::OnDestroy()
	{
	}

	void PopUpItem::SetPopUpParameters()
	{
		Wiwa::GuiManager& gm = m_Scene->GetGuiManager();
		Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();
		m_PopUp = true;
		m_PopOut = false;
		m_TimerIn = 0.0f;
		m_Animate = true;
		gm.canvas.at(11)->controls.at(2)->text = item.itemName;
		Text* newText = gm.InitFont("library/Fonts/Jade_Smile.ttf", item.itemName.c_str());
		r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(11)->controls.at(2)->id_quad_normal, 1);
		r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(11)->controls.at(2)->id_quad_normal, newText->GetTextureId());
		r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(11)->controls.at(2)->id_quad_normal, newText->GetSize(), { 0,0,512,512 });

		gm.canvas.at(11)->controls.at(3)->text = item.itemDescription;
		Text* newText2 = gm.InitFontForDialog("library/Fonts/Jade_Smile.ttf", (char*)item.itemDescription.c_str(),700);
		r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(11)->controls.at(3)->id_quad_normal, 1);
		r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(11)->controls.at(3)->id_quad_normal, newText2->GetTextureId());
		r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(11)->controls.at(3)->id_quad_normal, newText2->GetSize(), { 0,0,700,700 });
	}

	void PopUpItem::OnCollisionEnter(Object* body1, Object* body2)
	{
		Wiwa::GuiManager& gm = m_Scene->GetGuiManager();
		Wiwa::EntityManager& em = m_Scene->GetEntityManager();
		std::string itemStr = "ITEM";
		if (body1->id == m_EntityId && itemStr == body2->selfTagStr)
		{
			//gm.canvas.at(11)->SwapActive();
			Item* newItem = em.GetComponent<Item>(body2->id);
			item.itemName = newItem->Name;
			item.type = (Wiwa::ItemType)newItem->item_type;
			switch (item.type)
			{
			case ItemType::ABILITY:
				item.itemDescription = Wiwa::ItemManager::GetAbility(item.itemName.c_str())->Description;
				break;
			case ItemType::PASSIVE:
				item.itemDescription = Wiwa::ItemManager::GetPassive(item.itemName.c_str())->Description;
				break;
			case ItemType::BUFF:
				item.itemDescription = Wiwa::ItemManager::GetBuff(item.itemName.c_str())->Description;
				break;
			default:
				item.itemDescription = "NONE";
				break;
			}
			SetPopUpParameters();
		}
	}
}