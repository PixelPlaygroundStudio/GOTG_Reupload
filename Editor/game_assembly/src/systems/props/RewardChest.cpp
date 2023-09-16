#include "RewardChest.h"
#include "../../components/props/RewardChestData.h"
#include <Wiwa/ecs/systems/PhysicsSystem.h>
#include <Wiwa/core/Input.h>

void Wiwa::RewardChest::OnAwake()
{
	m_Face1 = m_Scene->GetEntityManager().GetChildByName(m_EntityId, "Face1");
	m_Face2 = m_Scene->GetEntityManager().GetChildByName(m_EntityId, "Face2");
	m_Face3 = m_Scene->GetEntityManager().GetChildByName(m_EntityId, "Face3");
	m_EntityChildren = *m_Scene->GetEntityManager().GetEntityChildren(m_EntityId);
	m_ColliderPreActive = m_Scene->GetEntityManager().GetChildByName(m_EntityId, "collider_item");
	m_Face1It = GetComponentIterator<Transform3D>(m_Face1);
	m_Face2It = GetComponentIterator<Transform3D>(m_Face2);
	m_Face3It = GetComponentIterator<Transform3D>(m_Face3);

	m_Velocity = 50.f;
	m_FinalPos = 100.f;

	m_ActiveAnim = false;
	m_Interacted = false;
}
void Wiwa::RewardChest::OnInit()
{
	if (!getAwake())
		OnAwake();

	Transform3D* face1 = GetComponentByIterator<Transform3D>(m_Face1It);
	Transform3D* face2 = GetComponentByIterator<Transform3D>(m_Face2It);
	Transform3D* face3 = GetComponentByIterator<Transform3D>(m_Face3It);

	m_Face1Start = face1->localRotation.x;
	m_Face2Start = face2->localRotation.x;
	m_Face3Start = face3->localRotation.x;

}
void Wiwa::RewardChest::OnUpdate()
{
	if (!getAwake())
		OnAwake();

	if (!getInit())
		OnInit();

	Transform3D* face1 = GetComponentByIterator<Transform3D>(m_Face1It);
	Transform3D* face2 = GetComponentByIterator<Transform3D>(m_Face2It);
	Transform3D* face3 = GetComponentByIterator<Transform3D>(m_Face3It);

	RewardChestData* data = GetComponent<RewardChestData>();

	if (m_ActiveAnim)
	{
		if (Wiwa::Input::IsButtonPressed(Gamepad::GamePad1, Key::GamepadA))
		{
			m_Interacted = true;
		}
		if (Wiwa::Input::IsButtonReleased(Gamepad::GamePad1, Key::GamepadA) && m_Interacted)
		{
			Wiwa::EntityManager& em = this->getScene().GetEntityManager();

			EntityId ChestParticlePrefabId = em.LoadPrefab("assets\\vfx\\prefabs\\vfx_finals\\p_openchest.wiprefab");
			if (ChestParticlePrefabId != WI_INVALID_INDEX)
			{
				Transform3D* ChestparticleTr = em.GetComponent<Transform3D>(ChestParticlePrefabId);
				ChestparticleTr->localPosition.x = -19.30;
				ChestparticleTr->localPosition.y = 2.0f;
				ChestparticleTr->localPosition.z = 25.50;
			}
			

			Wiwa::GuiManager& gm = m_Scene->GetGuiManager();
			data->Active = true;
			m_Scene->GetEntityManager().GetSystem<PhysicsSystem>(m_ColliderPreActive)->DeleteBody();
			m_Scene->GetEntityManager().GetSystem<PhysicsSystem>(m_EntityId)->DeleteBody();
			gm.canvas.at(9)->SwapActive();
			m_ActiveAnim = false;
			m_Interacted = false;
		}
	}
		
	if (data)
		if (!data->Active)
		{
			return;
		}
	if (face1->localRotation.x <= m_Face1Start + m_FinalPos)
	{
		face1->localRotation.x += m_Velocity * Time::GetDeltaTimeSeconds();
	}
	if (face2->localRotation.x <= m_Face2Start + m_FinalPos)
	{
		face2->localRotation.x += m_Velocity * Time::GetDeltaTimeSeconds();
	}
	if (face3->localRotation.x <= m_Face3Start + m_FinalPos)
	{
		face3->localRotation.x += m_Velocity * Time::GetDeltaTimeSeconds();
	}	
}

void Wiwa::RewardChest::OnCollisionEnter(Object* obj1, Object* obj2)
{
	Wiwa::GuiManager& gm = m_Scene->GetGuiManager();
	std::string playerTag = "PLAYER";

	if (obj1->id == m_EntityId && obj2->selfTagStr == playerTag)
	{
		gm.canvas.at(9)->SwapActive();
		m_ActiveAnim = true;
	}
}

void Wiwa::RewardChest::OnCollisionExit(Object* obj1, Object* obj2)
{
	Wiwa::GuiManager& gm = m_Scene->GetGuiManager();
	std::string playerTag = "PLAYER";

	if (obj1->id == m_EntityId && obj2->selfTagStr == playerTag)
	{
		gm.canvas.at(9)->SwapActive();
		m_ActiveAnim = false;
	}
}
