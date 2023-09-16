#include "ShipRotator.h"
#include "../../Components.h"
#include <Wiwa/utilities/render/CameraManager.h>
#include <Wiwa/utilities/render/Camera.h>

void Wiwa::ShipRotator::OnAwake()
{
}
void Wiwa::ShipRotator::OnInit()
{
	EntityId ship = m_Scene->GetEntityManager().GetChildByName(m_EntityId, "Ship");
	m_ShipDataIt = GetComponentIterator<ShipMainMenuData>(ship);
}
void Wiwa::ShipRotator::OnUpdate()
{
	ShipMainMenuData* data = GetComponentByIterator<ShipMainMenuData>(m_ShipDataIt);
	Camera* camera = m_Scene->GetCameraManager().getActiveCamera();
	glm::vec3 rot =
	{
		camera->getRotation().x + data->VelocityBackground * Time::GetDeltaTimeSeconds(),
		camera->getRotation().y,
		camera->getRotation().z
	};
	camera->setRotation(rot);

	GetTransform()->localRotation.y -= data->VelocityBackground * Time::GetDeltaTimeSeconds();
}
