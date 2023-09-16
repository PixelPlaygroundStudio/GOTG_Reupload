#include "HowardShopSystem.h"
#include <Wiwa/game/GameStateManager.h>
#include <Wiwa/core/Input.h>
#include <Wiwa/Ui/UiManager.h>
#include <Wiwa/game/Items/ItemManager.h>

namespace Wiwa
{

	Wiwa::HowardShopSystem::HowardShopSystem()
	{

	}

	HowardShopSystem::~HowardShopSystem()
	{
	}

	void HowardShopSystem::OnAwake()
	{
	}

	void HowardShopSystem::OnInit()
	{
		Wiwa::OzzAnimationSystem* animator = m_Scene->GetEntityManager().GetSystem<Wiwa::OzzAnimationSystem>(m_EntityId);

		animator->PlayAnimation("idle");
		m_ReturnToBaseHUD = false;
		m_ActiveSecondPage = false;
		m_FirstShopActive = false;
		m_SecondShopActive = false;
	}

	void HowardShopSystem::OnUpdate()
	{
		HandleCanvasShops();
		HandleTextPerButton();
	}

	void HowardShopSystem::OnDestroy()
	{
	}

	void HowardShopSystem::HandleCanvasShops()
	{
		Wiwa::GuiManager& gm = m_Scene->GetGuiManager();
		Wiwa::Character* character = Wiwa::GameStateManager::GetPlayerCharacterComp();
		if (m_FirstShopActive)
		{
			character->CanMove = false;
			if (Wiwa::Input::IsButtonPressed(Gamepad::GamePad1, Key::GamepadB))
			{
				m_ReturnToBaseHUD = true;
			}
			if (Wiwa::Input::IsButtonReleased(Gamepad::GamePad1, Key::GamepadB) && m_ReturnToBaseHUD)
			{
				gm.canvas.at(7)->SwapActive();
				gm.canvas.at(0)->SwapActive();
				character->CanMove = true;
				m_ReturnToBaseHUD = false;
				m_FirstShopActive = false;
			}

			if (Wiwa::Input::IsKeyPressed(Key::Backslash))
			{
				m_ReturnToBaseHUD = true;
			}
			if (Wiwa::Input::IsKeyReleased(Key::Backslash) && m_ReturnToBaseHUD)
			{
				gm.canvas.at(7)->SwapActive();
				gm.canvas.at(0)->SwapActive();
				character->CanMove = true;
				m_ReturnToBaseHUD = false;
				m_FirstShopActive = false;
			}

			if (Wiwa::Input::IsButtonPressed(Gamepad::GamePad1, Key::GamepadRigthBumper))
			{
				m_ActiveSecondPage = true;
			}
			if (Wiwa::Input::IsButtonReleased(Gamepad::GamePad1, Key::GamepadRigthBumper) && m_ActiveSecondPage)
			{
				gm.canvas.at(7)->SwapActive();
				gm.canvas.at(8)->SwapActive();
				m_ActiveSecondPage = false;
				m_FirstShopActive = false;
				m_SecondShopActive = true;
			}

			if (Wiwa::Input::IsKeyPressed(Key::Right))
			{
				m_ActiveSecondPage = true;
			}
			if (Wiwa::Input::IsKeyReleased(Key::Right) && m_ActiveSecondPage)
			{
				gm.canvas.at(7)->SwapActive();
				gm.canvas.at(8)->SwapActive();
				m_ActiveSecondPage = false;
				m_FirstShopActive = false;
				m_SecondShopActive = true;
			}
		}

		if (m_SecondShopActive)
		{
			character->CanMove = false;

			if (Wiwa::Input::IsButtonPressed(Gamepad::GamePad1, Key::GamepadLeftBumper))
			{
				m_ActiveSecondPage = true;
			}
			if (Wiwa::Input::IsButtonReleased(Gamepad::GamePad1, Key::GamepadLeftBumper) && m_ActiveSecondPage)
			{
				gm.canvas.at(8)->SwapActive();
				gm.canvas.at(7)->SwapActive();
				m_ActiveSecondPage = false;
				m_SecondShopActive = false;
				m_FirstShopActive = true;

			}

			if (Wiwa::Input::IsKeyPressed(Key::Left))
			{
				m_ActiveSecondPage = true;
			}
			if (Wiwa::Input::IsKeyReleased(Key::Left) && m_ActiveSecondPage)
			{
				gm.canvas.at(8)->SwapActive();
				gm.canvas.at(7)->SwapActive();
				m_ActiveSecondPage = false;
				m_SecondShopActive = false;
				m_FirstShopActive = true;

			}

			if (Wiwa::Input::IsButtonPressed(Gamepad::GamePad1, Key::GamepadB))
			{
				m_ReturnToBaseHUD = true;
			}
			if (Wiwa::Input::IsButtonReleased(Gamepad::GamePad1, Key::GamepadB) && m_ReturnToBaseHUD)
			{
				gm.canvas.at(8)->SwapActive();
				gm.canvas.at(0)->SwapActive();
				character->CanMove = true;
				m_ReturnToBaseHUD = false;
				m_SecondShopActive = false;
			}

			if (Wiwa::Input::IsKeyPressed(Key::Backslash))
			{
				m_ReturnToBaseHUD = true;
			}
			if (Wiwa::Input::IsKeyReleased(Key::Backslash) && m_ReturnToBaseHUD)
			{
				gm.canvas.at(8)->SwapActive();
				gm.canvas.at(0)->SwapActive();
				character->CanMove = true;
				m_ReturnToBaseHUD = false;
				m_SecondShopActive = false;
			}
		}
	}

	void HowardShopSystem::HandleTextPerButton()
	{
		Wiwa::GuiManager& gm = m_Scene->GetGuiManager();
		Wiwa::Renderer2D& r2d = Wiwa::Application::Get().GetRenderer2D();

		if (m_FirstShopActive && gm.canvas.at(7)->updateSelected)
		{
			size_t canvasSize = gm.canvas.at(7)->controlsForSelection.size();
			for (int i = 0; i < canvasSize; i++)
			{
				if (gm.canvas.at(7)->controlsForSelection.at(i)->GetState() == GuiControlState::FOCUSED)
				{
					Wiwa::ShopElement* shopElement = Wiwa::ItemManager::GetShopElement(itemsPerPage1[i]);
					int currentStepValue = shopElement->CurrentStep;
					if (shopElement->Costs.size() > currentStepValue)
					{
						std::string cost = std::to_string(shopElement->Costs.at(currentStepValue));
						gm.canvas.at(7)->controls.at(16)->text = cost;
						Text* newTextCoins = gm.InitFont("library/Fonts/Jade_Smile.ttf", cost.c_str());
						r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(7)->controls.at(16)->id_quad_normal, 1);
						r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(7)->controls.at(16)->id_quad_normal, newTextCoins->GetTextureId());
						r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(7)->controls.at(16)->id_quad_normal, newTextCoins->GetSize(), { 0,0,512,512 });
					}
					else
					{
						std::string cost = "MAX LEVEL";
						gm.canvas.at(7)->controls.at(16)->text = cost;
						Text* newTextCoins = gm.InitFont("library/Fonts/Jade_Smile.ttf", cost.c_str());
						r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(7)->controls.at(16)->id_quad_normal, 1);
						r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(7)->controls.at(16)->id_quad_normal, newTextCoins->GetTextureId());
						r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(7)->controls.at(16)->id_quad_normal, newTextCoins->GetSize(), { 0,0,512,512 });
					}

					std::string currentStep = std::to_string(currentStepValue);
					gm.canvas.at(7)->controls.at(17)->text = currentStep;
					Text* newTextCurrentStep = gm.InitFont("library/Fonts/Jade_Smile.ttf", currentStep.c_str());
					r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(7)->controls.at(17)->id_quad_normal, 1);
					r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(7)->controls.at(17)->id_quad_normal, newTextCurrentStep->GetTextureId());
					r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(7)->controls.at(17)->id_quad_normal, newTextCurrentStep->GetSize(), { 0,0,512,512 });

					std::string allSteps = std::to_string(shopElement->AmountOfSteps);
					gm.canvas.at(7)->controls.at(19)->text = allSteps;
					Text* newTextAllSteps = gm.InitFont("library/Fonts/Jade_Smile.ttf", allSteps.c_str());
					r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(7)->controls.at(19)->id_quad_normal, 1);
					r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(7)->controls.at(19)->id_quad_normal, newTextAllSteps->GetTextureId());
					r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(7)->controls.at(19)->id_quad_normal, newTextAllSteps->GetSize(), { 0,0,512,512 });
				}
			}
			
		}
		else if (m_SecondShopActive && gm.canvas.at(8)->updateSelected)
		{
			size_t canvasSize = gm.canvas.at(8)->controlsForSelection.size();
			for (int i = 0; i < canvasSize; i++)
			{
				if (gm.canvas.at(8)->controlsForSelection.at(i)->GetState() == GuiControlState::FOCUSED)
				{
					Wiwa::ShopElement* shopElement = Wiwa::ItemManager::GetShopElement(itemsPerPage2[i]);
					int currentStepValue = shopElement->CurrentStep;
					if (shopElement->Costs.size() > currentStepValue)
					{
						std::string cost = std::to_string(shopElement->Costs.at(currentStepValue));
						gm.canvas.at(8)->controls.at(10)->text = cost;
						Text* newTextCoins = gm.InitFont("library/Fonts/Jade_Smile.ttf", cost.c_str());
						r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(8)->controls.at(10)->id_quad_normal, 1);
						r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(8)->controls.at(10)->id_quad_normal, newTextCoins->GetTextureId());
						r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(8)->controls.at(10)->id_quad_normal, newTextCoins->GetSize(), { 0,0,512,512 });
					}
					else
					{
						std::string cost = "MAX LEVEL";
						gm.canvas.at(8)->controls.at(10)->text = cost;
						Text* newTextCoins = gm.InitFont("library/Fonts/Jade_Smile.ttf", cost.c_str());
						r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(8)->controls.at(10)->id_quad_normal, 1);
						r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(8)->controls.at(10)->id_quad_normal, newTextCoins->GetTextureId());
						r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(8)->controls.at(10)->id_quad_normal, newTextCoins->GetSize(), { 0,0,512,512 });
					}

					std::string currentStep = std::to_string(currentStepValue);
					gm.canvas.at(8)->controls.at(11)->text = currentStep;
					Text* newTextCurrentStep = gm.InitFont("library/Fonts/Jade_Smile.ttf", currentStep.c_str());
					r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(8)->controls.at(11)->id_quad_normal, 1);
					r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(8)->controls.at(11)->id_quad_normal, newTextCurrentStep->GetTextureId());
					r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(8)->controls.at(11)->id_quad_normal, newTextCurrentStep->GetSize(), { 0,0,512,512 });

					std::string allSteps = std::to_string(shopElement->AmountOfSteps);
					gm.canvas.at(8)->controls.at(13)->text = allSteps;
					Text* newTextAllSteps = gm.InitFont("library/Fonts/Jade_Smile.ttf", allSteps.c_str());
					r2d.UpdateInstancedQuadTexPriority(m_Scene, gm.canvas.at(8)->controls.at(13)->id_quad_normal, 1);
					r2d.UpdateInstancedQuadTexTexture(m_Scene, gm.canvas.at(8)->controls.at(13)->id_quad_normal, newTextAllSteps->GetTextureId());
					r2d.UpdateInstancedQuadTexClip(m_Scene, gm.canvas.at(8)->controls.at(13)->id_quad_normal, newTextAllSteps->GetSize(), { 0,0,512,512 });
				}
			}
		}
	}

	void HowardShopSystem::OnCollisionEnter(Object* body1, Object* body2)
	{
		Wiwa::GuiManager& gm = m_Scene->GetGuiManager();
		std::string playerStr = "PLAYER";
		if (body1->id == m_EntityId && playerStr == body2->selfTagStr)
		{
			gm.canvas.at(0)->SwapActive();
			gm.canvas.at(7)->SwapActive();
			m_FirstShopActive = true;
		}
	}
}