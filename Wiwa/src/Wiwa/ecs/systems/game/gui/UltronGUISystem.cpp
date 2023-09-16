#pragma once
#include "wipch.h"
#include "UltronGUISystem.h"
#include <Wiwa/core/Core.h>
#include <Wiwa/ecs/Systems.h>
#include <Wiwa/ecs/components/game/Health.h>
void Wiwa::UltronGUISystem::OnUpdate()
{
	if (!bossIsDead)
	{
		Wiwa::GuiManager& gm = m_Scene->GetGuiManager();
		Wiwa::EntityManager& em = m_Scene->GetEntityManager();
		Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();
		Wiwa::Health* health = em.GetComponent<Health>(m_EntityId);
		if (introBossFight)
		{
			if (!activeIntroCanvas)
			{
				if (Wiwa::GameStateManager::s_CurrentCharacter == 0)
				{
					//STARLORD
					gm.canvas.at(6)->controls.at(3)->SwapToNewTexture("library/hudimages/menus/gameovermenu_02/UI_BattleMenuStarlord_01.dds", Wiwa::Application::Get().GetRenderer2D());
					r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(6)->controls.at(3)->id_quad_normal, 1);
					UpdateBattleName("Starlord",gm);

				}
				else if (Wiwa::GameStateManager::s_CurrentCharacter == 1)
				{
					//ROCKET
					gm.canvas.at(6)->controls.at(3)->SwapToNewTexture("library/hudimages/menus/gameovermenu_02/UI_BattleMenuRocket_01.dds", Wiwa::Application::Get().GetRenderer2D());
					r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(6)->controls.at(3)->id_quad_normal, 1);
					UpdateBattleName("Rocket", gm);
				}
				m_Scene->SwapPauseActive();
				gm.canvas.at(0)->SwapActive();
				gm.canvas.at(6)->SwapActive();
				activeIntroCanvas = true;
			}
			introBossFight = AnimationIntroBoss(gm);
		}
		if (!introBossFight && !m_Scene->IsScenePaused())
		{
			if (!activeBossCanvas)
			{
				gm.canvas.at(6)->SwapActive();
				gm.canvas.at(5)->SwapActive();
				gm.canvas.at(0)->SwapActive();
				maxUltronHealth = health->health;
				activeBossCanvas = true;
			}
			if (activeBossCanvas)
			{
				if (gm.getCurrentCanvas() == 0 && !m_CanvasHUD)
				{
					gm.canvas.at(5)->SwapActive();
				}
				if (gm.getCurrentCanvas() == 0)
				{
					m_CanvasHUD = true;
				}
				if (gm.getCurrentCanvas() != 0 && m_CanvasHUD)
				{
					gm.canvas.at(5)->SwapActive();
					m_CanvasHUD = false;
				}


				if (health != nullptr)
				{
					if (health->health > 0)
					{
						gm.canvas.at(5)->controls.at(1)->SetValueForUIbar(health->health, maxUltronHealth);
					}
					else
					{
						gm.canvas.at(5)->SwapActive();
						activeBossCanvas = false;
						bossIsDead = true;
					}
				}
			}
		}
	}
	
}

bool Wiwa::UltronGUISystem::AnimationIntroBoss(Wiwa::GuiManager& gm)
{
	counterIntroAnim += 0.16f;
	//gm.canvas.at(6)->controls.at(8)->ScaleGUIElement({ 1024.0f,1024.0f }, 10, { 640.0f,640.0f }, GuiControlEasing::SineInOut);

	if (counterIntroAnim <= timeIntroAnim) 
	{
		return true;
	}
	else if(counterIntroAnim > timeIntroAnim && counterIntroAnim <= timeIntroAnim + 25 && !m_IterationForPause)
	{
		m_Scene->SwapPauseActive();
		m_IterationForPause = true;
		return true;
	}
	else if (counterIntroAnim > timeIntroAnim + 25)
	{
		return false;
	}

	return false;
}

void Wiwa::UltronGUISystem::OnCollisionEnter(Object* body1, Object* body2)
{
}

void Wiwa::UltronGUISystem::UpdateBattleName(std::string name,Wiwa::GuiManager& gm)
{
	Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();
	gm.canvas.at(6)->controls.at(0)->text = name.c_str();
	Text* newText = gm.InitFont("library/Fonts/Jade_Smile.ttf", name.c_str());
	r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(6)->controls.at(0)->id_quad_normal, 2);
	r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(6)->controls.at(0)->id_quad_normal, newText->GetTextureId());
	r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(6)->controls.at(0)->id_quad_normal, newText->GetSize(), { 0,0,512,512 });
}
