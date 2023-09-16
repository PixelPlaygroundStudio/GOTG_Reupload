#pragma once

#include <Wiwa/core/Core.h>
#include <Wiwa/core/Renderer2D.h>
#include <Wiwa/utilities/functions/Function.h>
#include <Wiwa/utilities/functions/Action.h>
#include <Wiwa/utilities/render/Text.h>
#include <vector>
#include <string>
#include <Wiwa/utilities/easings.h>
#include <Wiwa/utilities/time/Time.h>
#include <Wiwa/scene/Scene.h>

namespace Wiwa
{
	enum class GuiControlEasing
	{
		LinearIn,
		LinearOut,
		LinearInOut,
		SineIn,
		SineOut,
		SineInOut,
		CubicIn,
		CubicOut,
		CubicInOut,
		QuadIn,
		QuadOut,
		QuadInOut,
		ExpoIn,
		ExpoOut,
		ExpoInOut,
		EaseBackIn,
		EaseBackOut,
		EaseBackInOut,
		EaseBounceIn,
		EaseBounceOut,
		EaseBounceInOut,
		EaseElasticIn,
		EaseElasticOut,
		EaseElasticInOut,
	};
	enum class GuiControlType
	{
		BUTTON = 0,
		TEXT = 1,
		CHECKBOX = 3,
		SLIDER = 4,
		IMAGE = 6,
		BAR = 7,
		ABILITY = 8,
		VIDEO = 9,
	};

	enum class GuiControlState
	{
		DISABLED,
		NORMAL,
		FOCUSED,
		PRESSED,
		SELECTED
	};

	class WI_API GuiControl
	{
	protected:
		Scene* m_Scene;
	public:

		GuiControl(Scene* scene, GuiControlType type, unsigned int id) : m_Scene(scene), type(type), id(id), state(GuiControlState::NORMAL) {}

		GuiControl(Scene* scene, GuiControlType type, Rect2i bounds, char text[32]) : m_Scene(scene), type(type), state(GuiControlState::NORMAL)
		{
		}
		



		virtual bool Update()
		{
			return true;
		}

		virtual bool Draw(Renderer2D* render)
		{
			return true;
		}

		void SetTexture(Image* tex)
		{
			texture = tex;
		}
		void SetSliderTexture(Image* tex)
		{
			extraTexture = tex;
		}
		bool GetActive()
		{
			return active;
		}
		Rect2i GetPosition()
		{
			return position;
		}
		Rect2i GetExtraPosition()
		{
			return extraPosition;
		}
		GuiControlType GetType()
		{
			return type;
		}
		GuiControlState GetState()
		{
			return state;
		}
		Image* GetTexture()
		{
			return texture;
		}
		Image* GetExtraTexture()
		{
			return extraTexture;
		}

		Renderer2D::InstanceData GetExtraIdQuad()
		{
			return id_quad_extra;
		}

		void SetPosition_x(int position_x)
		{
			position.x = position_x;
			
		}
		void SetPosition_y(int position_y)
		{
			position.y = position_y;

		}
		void SetExtraPosition_x(int position_x)
		{
			extraPosition.x = position_x;

		}
		void SetExtraPosition_y(int position_y)
		{
			extraPosition.y = position_y;

		}
		void SetPosition_width(int position_width)
		{
			position.width = position_width;

		}
		void SetPosition_height(int position_height)
		{
			position.height = position_height;

		}

		void SwapActive(Wiwa::Renderer2D& r2d)
		{
			active = !active;
			if (active)
			{
				
				r2d.EnableInstance(m_Scene, id_quad_normal);
				
				

				if (type == GuiControlType::SLIDER)
				{
					r2d.EnableInstance(m_Scene, id_quad_extra);
				}
				
				
			}
			else
			{
				
					r2d.DisableInstance(m_Scene, id_quad_normal);
				

				if (type == GuiControlType::SLIDER)
				{
					r2d.DisableInstance(m_Scene, id_quad_extra);
				}
				
			}
		}
		bool SwapToNewTexture(const char* path, Wiwa::Renderer2D& r2d)
		{
			
			textId1 = Wiwa::Resources::Load<Wiwa::Image>(path);
			texture = Wiwa::Resources::GetResourceById<Wiwa::Image>(textId1);

			r2d.UpdateInstancedQuadTexTexture(m_Scene, id_quad_normal, texture->GetTextureId()); 
			return true;
		}

		void SwapText(const char* word, Wiwa::Renderer2D& r2d,Text* newText)
		{
			text = word;
			r2d.UpdateInstancedQuadTexTexture(m_Scene, id_quad_normal, newText->GetTextureId());
		}

		void SetValueForUIbar(float valueHealth,float valueMaxHealth)
		{
			if (type == GuiControlType::BAR)
			{
				float proportion = valueHealth / valueMaxHealth;
				position.width = (int)(proportion * extraPosition.width);
				texturePosition.width = position.width;
			}
		}

		void SetNextFrame(int value, Wiwa::Renderer2D* r2d)
		{
			if (animatedControl)
			{
				r2d->UpdateInstancedQuadTexClip(m_Scene, id_quad_normal, texture->GetSize(), positionsForAnimations.at(value));
			}
		}

		void HandleAnim(Wiwa::Renderer2D* r2d)
		{
			if (animatedControl)
			{
				timeForAnim += Wiwa::Time::GetDeltaTime();
				if (timeForAnim >= 1000/animSpeed)
				{
					if (framesAnimation < positionsForAnimations.size() - 1)
					{
						framesAnimation++;
					}
					else
					{
						framesAnimation = 0;
					}
					timeForAnim = 0.0f;
					r2d->UpdateInstancedQuadTexClip(m_Scene, id_quad_normal, texture->GetSize(), positionsForAnimations.at(framesAnimation));

				}
			}
		}

		void MoveGUIElement(Vector2f desired_position,float duration,Vector2f starting_position,GuiControlEasing easingType)
		{
			switch (easingType)
			{
			case Wiwa::GuiControlEasing::LinearIn:
				if (position.x < desired_position.x)
				{
					position.x = (int)EaseLinearIn(Wiwa::Time::GetDeltaTime(), starting_position.x, desired_position.x - starting_position.x, duration);
				}
				if (position.y < desired_position.y)
				{
					position.y = (int)EaseLinearIn(Wiwa::Time::GetDeltaTime(), starting_position.y, desired_position.y - starting_position.y, duration);
				}
				break;
			case Wiwa::GuiControlEasing::LinearOut:
				if (position.x < desired_position.x)
				{
					position.x = (int)EaseLinearOut(Wiwa::Time::GetDeltaTime(), starting_position.x, desired_position.x - starting_position.x, duration);
				}
				if (position.y < desired_position.y)
				{
					position.y = (int)EaseLinearOut(Wiwa::Time::GetDeltaTime(), starting_position.y, desired_position.y - starting_position.y, duration);
				}
				break;
			case Wiwa::GuiControlEasing::LinearInOut:
				if (position.x < desired_position.x)
				{
					position.x = (int)EaseLinearInOut(Wiwa::Time::GetDeltaTime(), starting_position.x, desired_position.x - starting_position.x, duration);
				}
				if (position.y < desired_position.y)
				{
					position.y = (int)EaseLinearInOut(Wiwa::Time::GetDeltaTime(), starting_position.y, desired_position.y - starting_position.y, duration);
				}
				break;
			case Wiwa::GuiControlEasing::SineIn:
				if (position.x < desired_position.x)
				{
					position.x = (int)EaseSineIn(Wiwa::Time::GetDeltaTime(), starting_position.x, desired_position.x - starting_position.x, duration);
				}
				if (position.y < desired_position.y)
				{
					position.y = (int)EaseSineIn(Wiwa::Time::GetDeltaTime(), starting_position.y, desired_position.y - starting_position.y, duration);
				}
				break;
			case Wiwa::GuiControlEasing::SineOut:
				if (position.x < desired_position.x)
				{
					position.x = (int)EaseSineOut(Wiwa::Time::GetDeltaTime(), starting_position.x, desired_position.x - starting_position.x, duration);
				}
				if (position.y < desired_position.y)
				{
					position.y = (int)EaseSineOut(Wiwa::Time::GetDeltaTime(), starting_position.y, desired_position.y - starting_position.y, duration);
				}
				break;
			case Wiwa::GuiControlEasing::SineInOut:
				if (position.x < desired_position.x)
				{
					position.x = (int)EaseSineInOut(Wiwa::Time::GetDeltaTime(), starting_position.x, desired_position.x - starting_position.x, duration);
				}
				if (position.y < desired_position.y)
				{
					position.y = (int)EaseSineInOut(Wiwa::Time::GetDeltaTime(), starting_position.y, desired_position.y - starting_position.y, duration);
				}
				break;
			case Wiwa::GuiControlEasing::CubicIn:
				if (position.x < desired_position.x)
				{
					position.x = (int)EaseCubicIn(Wiwa::Time::GetDeltaTime(), starting_position.x, desired_position.x - starting_position.x, duration);
				}
				if (position.y < desired_position.y)
				{
					position.y = (int)EaseCubicIn(Wiwa::Time::GetDeltaTime(), starting_position.y, desired_position.y - starting_position.y, duration);
				}
				break;
			case Wiwa::GuiControlEasing::CubicOut:
				if (position.x < desired_position.x)
				{
					position.x = (int)EaseCubicOut(Wiwa::Time::GetDeltaTime(), starting_position.x, desired_position.x - starting_position.x, duration);
				}
				if (position.y < desired_position.y)
				{
					position.y = (int)EaseCubicOut(Wiwa::Time::GetDeltaTime(), starting_position.y, desired_position.y - starting_position.y, duration);
				}
				break;
			case Wiwa::GuiControlEasing::CubicInOut:
				if (position.x < desired_position.x)
				{
					position.x = (int)EaseCubicInOut(Wiwa::Time::GetDeltaTime(), starting_position.x, desired_position.x - starting_position.x, duration);
				}
				if (position.y < desired_position.y)
				{
					position.y = (int)EaseCubicInOut(Wiwa::Time::GetDeltaTime(), starting_position.y, desired_position.y - starting_position.y, duration);
				}
				break;
			case Wiwa::GuiControlEasing::QuadIn:
				if (position.x < desired_position.x)
				{
					position.x = (int)EaseQuadIn(Wiwa::Time::GetDeltaTime(), starting_position.x, desired_position.x - starting_position.x, duration);
				}
				if (position.y < desired_position.y)
				{
					position.y = (int)EaseQuadIn(Wiwa::Time::GetDeltaTime(), starting_position.y, desired_position.y - starting_position.y, duration);
				}
				break;
			case Wiwa::GuiControlEasing::QuadOut:
				if (position.x < desired_position.x)
				{
					position.x = (int)EaseQuadOut(Wiwa::Time::GetDeltaTime(), starting_position.x, desired_position.x - starting_position.x, duration);
				}
				if (position.y < desired_position.y)
				{
					position.y = (int)EaseQuadOut(Wiwa::Time::GetDeltaTime(), starting_position.y, desired_position.y - starting_position.y, duration);
				}
				break;
			case Wiwa::GuiControlEasing::QuadInOut:
				if (position.x < desired_position.x)
				{
					position.x = (int)EaseQuadInOut(Wiwa::Time::GetDeltaTime(), starting_position.x, desired_position.x - starting_position.x, duration);
				}
				if (position.y < desired_position.y)
				{
					position.y = (int)EaseQuadInOut(Wiwa::Time::GetDeltaTime(), starting_position.y, desired_position.y - starting_position.y, duration);
				}
				break;
			case Wiwa::GuiControlEasing::ExpoIn:
				if (position.x < desired_position.x)
				{
					position.x = (int)EaseExpoIn(Wiwa::Time::GetDeltaTime(), starting_position.x, desired_position.x - starting_position.x, duration);
				}
				if (position.y < desired_position.y)
				{
					position.y = (int)EaseExpoIn(Wiwa::Time::GetDeltaTime(), starting_position.y, desired_position.y - starting_position.y, duration);
				}
				break;
			case Wiwa::GuiControlEasing::ExpoOut:
				if (position.x < desired_position.x)
				{
					position.x = (int)EaseExpoOut(Wiwa::Time::GetDeltaTime(), starting_position.x, desired_position.x - starting_position.x, duration);
				}
				if (position.y < desired_position.y)
				{
					position.y = (int)EaseExpoOut(Wiwa::Time::GetDeltaTime(), starting_position.y, desired_position.y - starting_position.y, duration);
				}
				break;
			case Wiwa::GuiControlEasing::ExpoInOut:
				if (position.x < desired_position.x)
				{
					position.x = (int)EaseExpoInOut(Wiwa::Time::GetDeltaTime(), starting_position.x, desired_position.x - starting_position.x, duration);
				}
				if (position.y < desired_position.y)
				{
					position.y = (int)EaseExpoInOut(Wiwa::Time::GetDeltaTime(), starting_position.y, desired_position.y - starting_position.y, duration);
				}
				break;
			case Wiwa::GuiControlEasing::EaseBackIn:
				if (position.x < desired_position.x)
				{
					position.x = (int)EaseBackIn(Wiwa::Time::GetDeltaTime(), starting_position.x, desired_position.x - starting_position.x, duration);
				}
				if (position.y < desired_position.y)
				{
					position.y = (int)EaseBackIn(Wiwa::Time::GetDeltaTime(), starting_position.y, desired_position.y - starting_position.y, duration);
				}
				break;
			case Wiwa::GuiControlEasing::EaseBackOut:
				if (position.x < desired_position.x)
				{
					position.x = (int)EaseBackOut(Wiwa::Time::GetDeltaTime(), starting_position.x, desired_position.x - starting_position.x, duration);
				}
				if (position.y < desired_position.y)
				{
					position.y = (int)EaseBackOut(Wiwa::Time::GetDeltaTime(), starting_position.y, desired_position.y - starting_position.y, duration);
				}
				break;
			case Wiwa::GuiControlEasing::EaseBackInOut:
				if (position.x < desired_position.x)
				{
					position.x = (int)EaseBackInOut(Wiwa::Time::GetDeltaTime(), starting_position.x, desired_position.x - starting_position.x, duration);
				}
				if (position.y < desired_position.y)
				{
					position.y = (int)EaseBackInOut(Wiwa::Time::GetDeltaTime(), starting_position.y, desired_position.y - starting_position.y, duration);
				}
				break;
			case Wiwa::GuiControlEasing::EaseBounceIn:
				if (position.x < desired_position.x)
				{
					position.x = (int)EaseBounceIn(Wiwa::Time::GetDeltaTime(), starting_position.x, desired_position.x - starting_position.x, duration);
				}
				if (position.y < desired_position.y)
				{
					position.y = (int)EaseBounceIn(Wiwa::Time::GetDeltaTime(), starting_position.y, desired_position.y - starting_position.y, duration);
				}
				break;
			case Wiwa::GuiControlEasing::EaseBounceOut:
				if (position.x < desired_position.x)
				{
					position.x = (int)EaseBounceOut(Wiwa::Time::GetDeltaTime(), starting_position.x, desired_position.x - starting_position.x, duration);
				}
				if (position.y < desired_position.y)
				{
					position.y = (int)EaseBounceOut(Wiwa::Time::GetDeltaTime(), starting_position.y, desired_position.y - starting_position.y, duration);
				}
				break;
			case Wiwa::GuiControlEasing::EaseBounceInOut:
				if (position.x < desired_position.x)
				{
					position.x = (int)EaseBounceInOut(Wiwa::Time::GetDeltaTime(), starting_position.x, desired_position.x - starting_position.x, duration);
				}
				if (position.y < desired_position.y)
				{
					position.y = (int)EaseBounceInOut(Wiwa::Time::GetDeltaTime(), starting_position.y, desired_position.y - starting_position.y, duration);
				}
				break;
			case Wiwa::GuiControlEasing::EaseElasticIn:
				if (position.x < desired_position.x)
				{
					position.x = (int)EaseElasticIn(Wiwa::Time::GetDeltaTime(), starting_position.x, desired_position.x - starting_position.x, duration);
				}
				if (position.y < desired_position.y)
				{
					position.y = (int)EaseElasticIn(Wiwa::Time::GetDeltaTime(), starting_position.y, desired_position.y - starting_position.y, duration);
				}
				break;
			case Wiwa::GuiControlEasing::EaseElasticOut:
				if (position.x < desired_position.x)
				{
					position.x = (int)EaseElasticOut(Wiwa::Time::GetDeltaTime(), starting_position.x, desired_position.x - starting_position.x, duration);
				}
				if (position.y < desired_position.y)
				{
					position.y = (int)EaseElasticOut(Wiwa::Time::GetDeltaTime(), starting_position.y, desired_position.y - starting_position.y, duration);
				}
				break;
			case Wiwa::GuiControlEasing::EaseElasticInOut:
				if (position.x < desired_position.x)
				{
					position.x = (int)EaseElasticInOut(Wiwa::Time::GetDeltaTime(), starting_position.x, desired_position.x - starting_position.x, duration);
				}
				if (position.y < desired_position.y)
				{
					position.y = (int)EaseElasticInOut(Wiwa::Time::GetDeltaTime(), starting_position.y, desired_position.y - starting_position.y, duration);
				}
				break;
			default:
				break;
			}
		}
		void SetInitialSliderValue(float value_desired)
		{
			value = value_desired;

			extraPosition.width = ((value * (float)position.width) / 100);
		}
		void ScaleGUIElement(Size2f desired_size, float duration, Size2f starting_size,GuiControlEasing easingType)
		{
			switch (easingType)
			{
			case Wiwa::GuiControlEasing::LinearIn:
				if (position.width < desired_size.w)
				{
					position.width = (int)EaseLinearIn(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				if (position.height < desired_size.h)
				{
					position.height = (int)EaseLinearIn(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);

				}
				break;
			case Wiwa::GuiControlEasing::LinearOut:
				if (position.width < desired_size.w)
				{
					position.width = (int)EaseLinearOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				if (position.height < desired_size.h)
				{
					position.height = (int)EaseLinearOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				break;
			case Wiwa::GuiControlEasing::LinearInOut:
				if (position.width < desired_size.w)
				{
					position.width = (int)EaseLinearInOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				if (position.height < desired_size.h)
				{
					position.height = (int)EaseLinearInOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				break;
			case Wiwa::GuiControlEasing::SineIn:
				if (position.width < desired_size.w)
				{
					position.width = (int)EaseSineIn(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				if (position.height < desired_size.h)
				{
					position.height = (int)EaseSineIn(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				break;
			case Wiwa::GuiControlEasing::SineOut:
				if (position.width < desired_size.w)
				{
					position.width = (int)EaseSineOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				if (position.height < desired_size.h)
				{
					position.height = (int)EaseSineOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				break;
			case Wiwa::GuiControlEasing::SineInOut:
				if (position.width < desired_size.w)
				{
					position.width = (int)EaseSineInOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				if (position.height < desired_size.h)
				{
					position.height = (int)EaseSineInOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				break;
			case Wiwa::GuiControlEasing::CubicIn:
				if (position.width < desired_size.w)
				{
					position.width = (int)EaseCubicIn(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				if (position.height < desired_size.h)
				{
					position.height = (int)EaseCubicIn(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				break;
			case Wiwa::GuiControlEasing::CubicOut:
				if (position.width < desired_size.w)
				{
					position.width = (int)EaseCubicOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				if (position.height < desired_size.h)
				{
					position.height = (int)EaseCubicOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				break;
			case Wiwa::GuiControlEasing::CubicInOut:
				if (position.width < desired_size.w)
				{
					position.width = (int)EaseCubicInOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				if (position.height < desired_size.h)
				{
					position.height = (int)EaseCubicInOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				break;
			case Wiwa::GuiControlEasing::QuadIn:
				if (position.width < desired_size.w)
				{
					position.width = (int)EaseQuadIn(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				if (position.height < desired_size.h)
				{
					position.height = (int)EaseQuadIn(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				break;
			case Wiwa::GuiControlEasing::QuadOut:
				if (position.width < desired_size.w)
				{
					position.width = (int)EaseQuadOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				if (position.height < desired_size.h)
				{
					position.height = (int)EaseQuadOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				break;
			case Wiwa::GuiControlEasing::QuadInOut:
				if (position.width < desired_size.w)
				{
					position.width = (int)EaseQuadInOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				if (position.height < desired_size.h)
				{
					position.height = (int)EaseQuadInOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				break;
			case Wiwa::GuiControlEasing::ExpoIn:
				if (position.width < desired_size.w)
				{
					position.width = (int)EaseExpoIn(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				if (position.height < desired_size.h)
				{
					position.height = (int)EaseExpoIn(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				break;
			case Wiwa::GuiControlEasing::ExpoOut:
				if (position.width < desired_size.w)
				{
					position.width = (int)EaseExpoOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				if (position.height < desired_size.h)
				{
					position.height = (int)EaseExpoOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				break;
			case Wiwa::GuiControlEasing::ExpoInOut:
				if (position.width < desired_size.w)
				{
					position.width = (int)EaseExpoInOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				if (position.height < desired_size.h)
				{
					position.height = (int)EaseExpoInOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				break;
			case Wiwa::GuiControlEasing::EaseBackIn:
				if (position.width < desired_size.w)
				{
					position.width = (int)EaseBackIn(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				if (position.height < desired_size.h)
				{
					position.height = (int)EaseBackIn(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				break;
			case Wiwa::GuiControlEasing::EaseBackOut:
				if (position.width < desired_size.w)
				{
					position.width = (int)EaseBackOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				if (position.height < desired_size.h)
				{
					position.height = (int)EaseBackOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				break;
			case Wiwa::GuiControlEasing::EaseBackInOut:
				if (position.width < desired_size.w)
				{
					position.width = (int)EaseBackInOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				if (position.height < desired_size.h)
				{
					position.height = (int)EaseBackInOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				break;
			case Wiwa::GuiControlEasing::EaseBounceIn:
				if (position.width < desired_size.w)
				{
					position.width = (int)EaseBounceIn(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				if (position.height < desired_size.h)
				{
					position.height = (int)EaseBounceIn(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				break;
			case Wiwa::GuiControlEasing::EaseBounceOut:
				if (position.width < desired_size.w)
				{
					position.width = (int)EaseBounceOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				if (position.height < desired_size.h)
				{
					position.height = (int)EaseBounceOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				break;
			case Wiwa::GuiControlEasing::EaseBounceInOut:
				if (position.width < desired_size.w)
				{
					position.width = (int)EaseBounceInOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				if (position.height < desired_size.h)
				{
					position.height = (int)EaseBounceInOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				break;
			case Wiwa::GuiControlEasing::EaseElasticIn:
				if (position.width < desired_size.w)
				{
					position.width = (int)EaseElasticIn(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				if (position.height < desired_size.h)
				{
					position.height = (int)EaseElasticIn(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				break;
			case Wiwa::GuiControlEasing::EaseElasticOut:
				if (position.width < desired_size.w)
				{
					position.width = (int)EaseElasticOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				if (position.height < desired_size.h)
				{
					position.height = (int)EaseElasticOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				break;
			case Wiwa::GuiControlEasing::EaseElasticInOut:
				if (position.width < desired_size.w)
				{
					position.width = (int)EaseElasticInOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				if (position.height < desired_size.h)
				{
					position.height = (int)EaseElasticInOut(Wiwa::Time::GetDeltaTime(), starting_size.w, desired_size.w - starting_size.w, duration);
				}
				break;
			default:
				break;
			}

			texturePosition.width = position.width;
			texturePosition.height = position.height;
			extraPosition.width = position.width;
			extraPosition.height = position.height;
			extraTexturePosition.width = extraTexturePosition.width;
			extraTexturePosition.height = extraTexturePosition.height;

		}


		void SetChecked(bool ret) { checked = ret; }
	public:

		ResourceId textId1;
		ResourceId textId2;

		bool active;
		int id;
		GuiControlType type;
		GuiControlState state;

		std::string text;           
		std::string audioEventForButton;
		std::string audioEventFocused;
		std::string audioEventSelected;
		Rect2i position;        
		Rect2i texturePosition;        
		Rect2i extraPosition;	
		Rect2i extraTexturePosition;        
		Color4f color = Color::WHITE;        

		Image* texture;
		Image* extraTexture;	

		Renderer2D::InstanceData id_quad_normal;

		Renderer2D::InstanceData id_quad_extra;

		size_t callbackID;

		std::string name;
		const Func* callback;

		//THINGS FOR ANIMATIONS
		std::vector<Rect2i> positionsForAnimations;
		Rect2i startPos;
		bool animatedControl = false; 
		size_t framesAnimation;
		float animSpeed;
		float timeForAnim = 0.0f;
		float rotation = 0.0f;
		float currentAnimTime =0.0f;
		bool Animating = false;

		float value = 50.0f;
		bool checked = false;
		bool Focused = false;
	};
}