#include "VentRotator.h"
#include "../../Components/props/VentRotatorData.h"

void Wiwa::VentRotator::OnInit()
{
	m_VentRotatorIt = GetComponentIterator<VentRotatorData>();
	VentRotatorData* data = GetComponentByIterator<VentRotatorData>(m_VentRotatorIt);
	m_Velocity = RANDF(data->Min, data->Max);
	data->Velocity = m_Velocity;
}

void Wiwa::VentRotator::OnUpdate()
{
	VentRotatorData* data = GetComponentByIterator<VentRotatorData>(m_VentRotatorIt);

	if (data)
	{
		if (data->IsLeft)
		{
			GetTransform()->localRotation.y += m_Velocity * Time::GetDeltaTimeSeconds();
			return;
		}
		else if (data->IsRight)
		{
			GetTransform()->localRotation.z += m_Velocity * Time::GetDeltaTimeSeconds();
			return;
		}
	}
	GetTransform()->localRotation.z += m_Velocity * Time::GetDeltaTimeSeconds();
}
