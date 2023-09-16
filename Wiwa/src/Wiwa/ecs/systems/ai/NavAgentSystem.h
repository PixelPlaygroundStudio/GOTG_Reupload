#pragma once
#include <Wiwa/ecs/systems/System.h>
#include "Wiwa/AI/AI_Crowd.h"
#include <glm/glm.hpp>
#include <Wiwa/ecs/EntityManager.h>
#include <Wiwa/ecs/components/ai/NavAgent.h>

namespace Wiwa
{
    class WI_API NavAgentSystem : public System {
    public:
        NavAgentSystem();

        ~NavAgentSystem();

        void OnInit() override;

        void OnUpdate() override;

        void SetDestination(const glm::vec3& target);
        void SetPosition(const glm::vec3& position);

        void SetMaxSpeed(float maxSpeed);
        void SetMaxAcceleration(float maxAcceleration);

        void SetPreviousMaxSpeed();
        void SetPreviousMaxAcceleration();

        void RequestMoveVelocity(const glm::vec3& velocity);
        void StopAgent();
        void StopAgentAndVelocity();
        void RemoveAgent();

        bool Raycast(const glm::vec3& start_point, const glm::vec3& start_end);

        const glm::vec3& GetCurrentPosition() const;
        //const glm::vec3& GetCurrentVelocity() const;
        const glm::vec3 GetCurrrentRotation(const glm::vec3& current_transform_rotation) const;
        float GetMaxSpeed() const;
        float GetMaxAcceleration() const;

        void RegisterWithCrowd();
        void RefreshParamters();

        glm::vec3 GetRandPointOutsideCircle(const glm::vec3& circle_position, float radius);

        bool OnEnabledFromPool() override;

        bool OnDisabledFromPool() override;

        int getAgentIndex() const { return m_AgentIndex; };
        float m_RotationValue;

    private:
        void Render();
        void DrawAgent();
        void DrawPath();

        int m_AgentIndex;
        glm::vec3 m_CurrentPos;
        float m_PreviousVelocity;
        float m_PreviousAcceleration;
        dtCrowdAgentParams m_AgentParams;
        EntityManager::ComponentIterator m_NavAgentIt;

        bool isPoolActive = true;
        //EntityManager::ComponentIterator m_Transform;
    };

}

REGISTER_SYSTEM(Wiwa::NavAgentSystem);