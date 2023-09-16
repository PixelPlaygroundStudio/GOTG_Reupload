#include "BossMovementPointsSystem.h"
#include "../../components/enemy/BossMovementPoints.h"

Wiwa::BossMovementPointsSystems::BossMovementPointsSystems()
{
	m_PointIt = { WI_INVALID_INDEX, WI_INVALID_INDEX };
}

Wiwa::BossMovementPointsSystems::~BossMovementPointsSystems()
{
}

void Wiwa::BossMovementPointsSystems::OnAwake()
{
	m_PointIt = GetComponentIterator<BossMovementPoints>();
	BossMovementPoints* point = GetComponentByIterator<BossMovementPoints>(m_PointIt);
	point->point = GetTransform()->localPosition;
}
