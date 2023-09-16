#include <wipch.h>
#include "SubjugatorChasing.h"
#include "../EnemySubjugator.h"
#include <Wiwa/ecs/systems/OzzAnimationSystem.h>
#include <Wiwa/ecs/systems/ai/NavAgentSystem.h>

glm::vec3 CalculateOffsetPositionSubjugator(const glm::vec3& playerPosition, int totalEnemies, float radius)
{
    // Randomize the enemy index
    int enemyIndex = rand() % totalEnemies;

    float angle = 360.0f / totalEnemies;
    float offsetX = radius * cos(glm::radians(angle * enemyIndex));
    float offsetZ = radius * sin(glm::radians(angle * enemyIndex));
    return playerPosition + glm::vec3(offsetX, 0.0f, offsetZ);
}


namespace Wiwa
{
    SubjugatorChasingState::SubjugatorChasingState()
    {

    }

    SubjugatorChasingState::~SubjugatorChasingState()
    {

    }

    void SubjugatorChasingState::EnterState(EnemySubjugator* enemy)
    {
        Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();
        Transform3D* playerTr = (Transform3D*)em.GetComponentByIterator(enemy->m_PlayerTransformIt);
        //offsetPosition = CalculateOffsetPosition(playerTr->localPosition, 50, enemy->m_RangeOfAttack * 0.8f);
        enemy->m_AnimatorSys->PlayAnimation("walk");

        //m_HasTargetPoint = false;
        m_TargetPoint = glm::vec3(0.0f, 0.0f, 0.0f);

    }

    void SubjugatorChasingState::UpdateState(EnemySubjugator* enemy)
    {
        m_ChasingTimer += Time::GetDeltaTime();

        bool remakeOffsetPosition = false;

        // Check if player is in range
        Wiwa::EntityManager& em = enemy->getScene().GetEntityManager();

        Transform3D* selfTr = (Transform3D*)em.GetComponentByIterator(enemy->m_TransformIt);
        Transform3D* playerTr = (Transform3D*)em.GetComponentByIterator(enemy->m_PlayerTransformIt);
        EnemyData* stats = (EnemyData*)em.GetComponentByIterator(enemy->m_StatsIt);

        float distanceToPlayer = glm::distance(playerTr->localPosition, selfTr->localPosition);

        if (enemy->m_NavAgentSys != nullptr)
        {
            m_TargetPoint = playerTr->localPosition;
            enemy->m_NavAgentSys->SetDestination(m_TargetPoint);
        }

        if (distanceToPlayer < stats->range && enemy->m_NavAgentSys->Raycast(selfTr->localPosition, playerTr->localPosition))
        {
            enemy->m_NavAgentSys->StopAgent();
            enemy->SwitchState(enemy->m_AttackingState);
        }
    }


    void SubjugatorChasingState::ExitState(EnemySubjugator* enemy)
    {

    }

    void SubjugatorChasingState::OnCollisionEnter(EnemySubjugator* enemy, const Object* body1, const Object* body2)
    {

    }
}