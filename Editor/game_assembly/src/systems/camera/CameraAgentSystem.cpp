#include "CameraAgentSystem.h"
#include "../../components/camera/CameraAgent.h"

void Wiwa::CameraAgentSystem::OnAwake()
{
	m_DataIt.c_id = WI_INVALID_INDEX;
}
void Wiwa::CameraAgentSystem::OnInit()
{
	m_DataIt = GetComponentIterator<CameraAgent>();
}
void Wiwa::CameraAgentSystem::OnUpdate()
{
	if (!getInit())
		OnInit();
	if (m_DataIt.c_id == WI_INVALID_INDEX)
	{
		m_DataIt = GetComponentIterator<CameraAgent>();
	}
	if (m_DataIt.c_id != WI_INVALID_INDEX)
	{
		CameraAgent* data = GetComponentByIterator<CameraAgent>(m_DataIt);
		data->Position = Math::GetWorldPosition(GetTransform()->worldMatrix);

	}
}

bool Wiwa::CameraAgentSystem::OnEnabledFromPool()
{
	if (!getInit())
		OnInit();
	if (m_DataIt.c_id == WI_INVALID_INDEX)
	{
		m_DataIt = GetComponentIterator<CameraAgent>();
	}
	if (m_DataIt.c_id != WI_INVALID_INDEX)
	{
		CameraAgent* data = GetComponentByIterator<CameraAgent>(m_DataIt);
		data->Active = true;
	}
	return true;
}

bool Wiwa::CameraAgentSystem::OnDisabledFromPool()
{
	if (!getInit())
		OnInit();
	if (m_DataIt.c_id == WI_INVALID_INDEX)
	{
		m_DataIt = GetComponentIterator<CameraAgent>();
	}
	if (m_DataIt.c_id != WI_INVALID_INDEX)
	{
		CameraAgent* data = GetComponentByIterator<CameraAgent>(m_DataIt);
		data->Active = false;
	}
	return true;
}

