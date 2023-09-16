#include <wipch.h>
#include "NavAgentSystem.h"
#include "Wiwa/ecs/components/ai/NavAgent.h"
#include <glew.h>
#include <Wiwa/ecs/systems/PhysicsSystem.h>
#include <glm/gtx/vector_angle.hpp> // for glm::angle()

namespace Wiwa
{
    NavAgentSystem::NavAgentSystem()
    {
        m_AgentIndex = -1;
      
        //m_AgentParams.radius = ;
        //m_AgentParams.height = m_height;
        //m_AgentParams.maxSpeed = m_maxSpeed;
        //m_AgentParams.maxAcceleration = m_maxAcceleration;
        //m_AgentParams.collisionQueryRange = m_radius * 12.f;
        //m_AgentParams.pathOptimizationRange = m_radius * 30.f;
        //m_AgentParams.updateFlags = DT_CROWD_ANTICIPATE_TURNS | DT_CROWD_OPTIMIZE_VIS | DT_CROWD_OPTIMIZE_TOPO | DT_CROWD_OBSTACLE_AVOIDANCE;
        //m_AgentParams.obstacleAvoidanceType = 3;
        //m_AgentParams.queryFilterType = 0;
        //m_AgentParams.separationWeight = 1;
        m_PreviousVelocity = 0.0f;
        m_PreviousAcceleration = 0.0f;
        m_RotationValue = 30.0f;
    }

    NavAgentSystem::~NavAgentSystem()
    {
        if (m_AgentIndex != -1) {
            Crowd::getInstance().RemoveAgent(m_AgentIndex);
        }
    }

    void NavAgentSystem::OnInit()
    {
        if (GetEntityManager().IsActive(m_EntityId) == false)
        {
            m_AgentIndex = -1;
            return;
        }

        Transform3D* tr = GetComponent<Transform3D>();
        m_CurrentPos = tr->localPosition;
        m_NavAgentIt = GetComponentIterator<Wiwa::NavAgent>();
        Wiwa::NavAgent* agent = GetComponentByIterator<Wiwa::NavAgent>(m_NavAgentIt);
        agent->agentSliding = true;
        RefreshParamters();
        RegisterWithCrowd();
    }

    // Returns a random number [0..1]
    static float frand()
    {
        //	return ((float)(rand() & 0xffff)/(float)0xffff);
        return (float)rand() / (float)RAND_MAX;
    }

    void NavAgentSystem::OnUpdate()
    {
        if (m_AgentIndex == -1)
            return;

        RefreshParamters();
        Crowd& crowd = Crowd::getInstance();
       /* const dtQueryFilter* filter= Crowd::getInstance().getCrowd().getFilter(m_AgentIndex);
        const dtNavMeshQuery* query = Crowd::getInstance().getCrowd().getNavMeshQuery();
        dtPolyRef startRef;
        float m_spos[3];
        dtStatus status = query->findRandomPoint(filter, frand, &startRef, m_spos);
        if (dtStatusSucceed(status))
        {
            SetDestination(glm::vec3(m_spos[0], m_spos[1], m_spos[2]));
        }*/

        if (m_AgentIndex != -1) {
            const dtCrowdAgent* crowdAgent = crowd.getCrowd().getAgent(m_AgentIndex);
            Wiwa::NavAgent* navAgent = GetComponentByIterator<Wiwa::NavAgent>(m_NavAgentIt);

            if (crowdAgent)
            {
                m_CurrentPos.x = crowdAgent->npos[0];
                m_CurrentPos.y = crowdAgent->npos[1];
                m_CurrentPos.z = crowdAgent->npos[2];

                // Calculate distance to target
                float distanceToTarget = glm::distance(m_CurrentPos, glm::vec3(crowdAgent->targetPos[0], crowdAgent->targetPos[1], crowdAgent->targetPos[2]));

                if (distanceToTarget > navAgent->stoppingDistance) {
                    // Set agent target and speed
                    crowd.SetAgentMaxAcceleration(m_AgentIndex, m_AgentParams.maxAcceleration);
                    crowd.SetAgentMaxSpeed(m_AgentIndex, m_AgentParams.maxSpeed);
                }
                else {
                    // Slow down agent as it approaches target
                    float deceleration = (m_AgentParams.maxSpeed * m_AgentParams.maxSpeed) / (2 * navAgent->stoppingDistance);
                    float desiredSpeed = (distanceToTarget / navAgent->stoppingDistance) * m_AgentParams.maxSpeed;
                    float speed = std::min(desiredSpeed, m_AgentParams.maxSpeed);
                    speed = std::max(speed, 0.0f);
                    crowd.SetAgentMaxAcceleration(m_AgentIndex, m_AgentParams.maxAcceleration);
                    crowd.SetAgentMaxSpeed(m_AgentIndex, speed);
                }
            }

            if (!navAgent->agentSliding)
            {
                dtCrowdAgent* agent = crowd.getCrowd().getEditableAgent(m_AgentIndex);
                if (agent)
                {
                    agent->vel[0] = agent->dvel[0];
                    agent->vel[1] = agent->dvel[1];
                    agent->vel[2] = agent->dvel[2];
                }
            }

            Render();

            // Move the target
            Wiwa::EntityManager& em = m_Scene->GetEntityManager();
            Wiwa::PhysicsSystem* physSys = em.GetSystem<PhysicsSystem>(m_EntityId);

            Transform3D* tr = GetComponent<Transform3D>();
            tr->localPosition = m_CurrentPos;

            // Rotate the target
            if (navAgent->autoRotate)
            {
                glm::vec3 rotation = GetCurrrentRotation(tr->localRotation);
                tr->localRotation = rotation;
            }
        }
    }

    void NavAgentSystem::SetDestination(const glm::vec3& target)
    {
        if (m_AgentIndex != -1) {
            Crowd::getInstance().SetAgentTarget(m_AgentIndex, &target[0]);
        }
    }

    void NavAgentSystem::SetPosition(const glm::vec3& position)
    {
        if (m_AgentIndex != -1) {
            dtCrowdAgent* agent = Crowd::getInstance().getCrowd().getEditableAgent(m_AgentIndex);
            Crowd::getInstance().getCrowd().resetMoveTarget(m_AgentIndex);
            if (agent)
            {
                agent->npos[0] = position.x;
                agent->npos[1] = position.y;
                agent->npos[2] = position.z;
            }

            m_CurrentPos = position;
        }
    }

    void NavAgentSystem::SetMaxSpeed(float maxSpeed)
    {
        m_PreviousVelocity = m_AgentParams.maxSpeed;
        m_AgentParams.maxSpeed = maxSpeed;
        if (m_AgentIndex != -1) {
            Crowd::getInstance().getCrowd().updateAgentParameters(m_AgentIndex, &m_AgentParams);
        }
    }

    void NavAgentSystem::SetMaxAcceleration(float maxAcceleration)
    {
        m_PreviousAcceleration = m_AgentParams.maxAcceleration;
        m_AgentParams.maxAcceleration = maxAcceleration;
        if (m_AgentIndex != -1) {
            Crowd::getInstance().getCrowd().updateAgentParameters(m_AgentIndex, &m_AgentParams);
        }
    }

    void NavAgentSystem::SetPreviousMaxSpeed()
    {
        m_AgentParams.maxSpeed = m_PreviousVelocity;
    }

    void NavAgentSystem::SetPreviousMaxAcceleration()
    {
        m_AgentParams.maxAcceleration = m_PreviousAcceleration;
    }

    void NavAgentSystem::RequestMoveVelocity(const glm::vec3& velocity)
    {
        if (m_AgentIndex != -1) {
            Crowd::getInstance().getCrowd().requestMoveVelocity(m_AgentIndex, &velocity[0]);
        }
    }

    void NavAgentSystem::StopAgent()
    {
        if (m_AgentIndex != -1) {
            Crowd& crowd = Crowd::getInstance();
            crowd.getCrowd().resetMoveTarget(m_AgentIndex);
        }
    }

    void NavAgentSystem::StopAgentAndVelocity()
    {
        if (m_AgentIndex != -1) {
            Crowd& crowd = Crowd::getInstance();
            crowd.getCrowd().resetMoveTarget(m_AgentIndex);
            dtCrowdAgent* agent = crowd.getCrowd().getEditableAgent(m_AgentIndex);
            if (agent)
            {
                agent->vel[0] = agent->dvel[0];
                agent->vel[1] = agent->dvel[1];
                agent->vel[2] = agent->dvel[2];
            }
        }
    }

    void NavAgentSystem::RemoveAgent()
    {
        if (m_AgentIndex != -1) {
            Crowd::getInstance().RemoveAgent(m_AgentIndex);
            m_AgentIndex = -1;
        }
    }

    bool NavAgentSystem::Raycast(const glm::vec3& start_point, const glm::vec3& end_point)
    {
        Crowd& crowd = Crowd::getInstance();
        dtCrowd& dtCrowd = crowd.getCrowd();
        const dtNavMeshQuery* navMeshQuery = dtCrowd.getNavMeshQuery();

        dtPolyRef startRef;
        float targetPos[3];
        const dtQueryFilter* filter = dtCrowd.getFilter(m_AgentIndex);
        const float* extents = dtCrowd.getQueryExtents();
        navMeshQuery->findNearestPoly(&start_point[0], extents, filter, &startRef, targetPos);

        glm::vec3 pt(0.0f);
        dtPolyRef ref;
        float distance = 0.0f;
        float t;
        float hitNormal[3];
        dtPolyRef path[6];
        int count;
        const int maxPath = 6;
        navMeshQuery->raycast(startRef, &start_point[0], &end_point[0], filter, &t, hitNormal, path, &count, maxPath);
        if (t > 1)
        {
            // Hit
            //WI_INFO("HITTTTT");
            return true;
        }
        else
        {
            // No hit
            //WI_INFO("NO HIT");
            return false;
        }
        return false;
       /* if (dtStatusSucceed(status))
        {
            if (hit.t == FLT_MAX)
                return false;

            return true;
        }
        return false;*/
    }

    const glm::vec3& NavAgentSystem::GetCurrentPosition() const
    {
        return m_CurrentPos;
    }

  /*  const glm::vec3& NavAgentSystem::GetCurrentVelocity() const
    {
        return m_CurrentVel;
    }*/

    const glm::vec3 NavAgentSystem::GetCurrrentRotation(const glm::vec3& current_transform_rotation) const
    {
        if (m_AgentIndex != -1) {
            Crowd& crowd = Crowd::getInstance();
            const dtCrowdAgent* crowdAgent = crowd.getCrowd().getAgent(m_AgentIndex);

            glm::vec3 nextPos(crowdAgent->cornerVerts[0], crowdAgent->cornerVerts[1], crowdAgent->cornerVerts[2]);

            float distance = glm::distance(m_CurrentPos, nextPos);

            float timeToRotate = distance / m_RotationValue;
            float tRot = glm::clamp(Time::GetDeltaTimeSeconds() / timeToRotate, 0.0f, 1.0f);

            // Calculate the forward vector from the current position to the target position
            glm::vec3 forward = glm::normalize(nextPos - m_CurrentPos);

            // Calculate the angle between the forward vector and the world forward vector (0, 0, 1)
            float angle = glm::angle(forward, glm::vec3(0.0f, 0.0f, 1.0f));

            // Determine the sign of the angle based on the cross product between the forward vector and the world forward vector (0, 0, 1)
            glm::vec3 crossProduct = glm::cross(forward, glm::vec3(0.0f, 0.0f, 1.0f));
            if (crossProduct.y > 0.0f) {
                angle = -angle;
            }

            float targetRotation = angle * 180 / glm::pi<float>();

            // Calculate the difference between the current rotation and target rotation
            float rotationDifference = targetRotation - current_transform_rotation.y;

            // Adjust the rotation difference to be within the range of -180 to 180 degrees
            while (rotationDifference > 180.0f) {
                rotationDifference -= 360.0f;
            }
            while (rotationDifference < -180.0f) {
                rotationDifference += 360.0f;
            }

            // Calculate the new interpolated rotation
            float interpolatedRotation = current_transform_rotation.y + rotationDifference * tRot;

            return glm::vec3(current_transform_rotation.x, interpolatedRotation, current_transform_rotation.z);
        }
        return current_transform_rotation;
    }

    float NavAgentSystem::GetMaxSpeed() const
    {
        return m_AgentParams.maxSpeed;
    }

    float NavAgentSystem::GetMaxAcceleration() const
    {
        return m_AgentParams.maxAcceleration;
    }

    void NavAgentSystem::RegisterWithCrowd()
    {
        if (m_AgentIndex != -1) {
            Crowd::getInstance().RemoveAgent(m_AgentIndex);
        }

        m_AgentIndex = Crowd::getInstance().AddAgent(&m_CurrentPos[0], &m_AgentParams);
        Crowd::getInstance().SetAgentParameters(m_AgentIndex, m_AgentParams);
    }

    void NavAgentSystem::RefreshParamters()
    {
        Wiwa::NavAgent* agent = GetComponentByIterator<Wiwa::NavAgent>(m_NavAgentIt);
        if (agent)
        {
            m_AgentParams.radius = agent->radius;
            m_AgentParams.height = agent->height;
            m_AgentParams.maxSpeed = agent->maxSpeed;
            m_AgentParams.maxAcceleration = agent->maxAcceleration;
            m_AgentParams.collisionQueryRange = agent->radius * 12.f;
            m_AgentParams.pathOptimizationRange = agent->radius * 30.f;
            m_AgentParams.updateFlags = DT_CROWD_ANTICIPATE_TURNS | DT_CROWD_OPTIMIZE_VIS | DT_CROWD_OPTIMIZE_TOPO | DT_CROWD_OBSTACLE_AVOIDANCE;
            m_AgentParams.obstacleAvoidanceType = 3;
            m_AgentParams.queryFilterType = 0;
            m_AgentParams.separationWeight = 10;
        }
    }

    glm::vec3 NavAgentSystem::GetRandPointOutsideCircle(const glm::vec3& circle_position, float radius)
    {
        Crowd& crowd = Crowd::getInstance();
        dtCrowd& dtCrowd = crowd.getCrowd();
        const dtNavMeshQuery* navMeshQuery = dtCrowd.getNavMeshQuery();

        dtPolyRef targetRef;
        float targetPos[3];
        const dtQueryFilter* filter = dtCrowd.getFilter(m_AgentIndex);
        const float* extents = dtCrowd.getQueryExtents();
        navMeshQuery->findNearestPoly(&circle_position[0], extents, filter, &targetRef, targetPos);

        glm::vec3 pt(0.0f);
        dtPolyRef ref;
        float distance = 0.0f;

        do
        {
            dtStatus status = navMeshQuery->findRandomPointAroundCircle(targetRef, &circle_position[0], radius, filter, frand, &ref, &pt[0]);
            if (dtStatusSucceed(status))
            {
                float distance = glm::distance(circle_position, pt);
            }
        } while (distance > radius);

        return pt;
    }

    bool NavAgentSystem::OnEnabledFromPool()
    {
        OnInit();
        return true;
    }

    bool NavAgentSystem::OnDisabledFromPool()
    {
        if (m_AgentIndex != -1) {
            Crowd::getInstance().RemoveAgent(m_AgentIndex);
        }
        m_AgentIndex = -1;
        return true;
    }

    void NavAgentSystem::Render()
    {
        Wiwa::Camera* camera = Wiwa::SceneManager::getActiveScene()->GetCameraManager().editorCamera;

        if (!camera->drawBoundingBoxes)
            return;

        GL(Viewport(0, 0, camera->frameBuffer->getWidth(), camera->frameBuffer->getHeight()));
        camera->frameBuffer->Bind(false);

        GL(MatrixMode(GL_PROJECTION));
        GL(LoadMatrixf(glm::value_ptr(camera->getProjection())));
        GL(MatrixMode(GL_MODELVIEW));
        GL(LoadMatrixf(glm::value_ptr(camera->getView())));

        DrawAgent();

        DrawPath();

        camera->frameBuffer->Unbind();
    }

    void NavAgentSystem::DrawAgent()
    {
        const unsigned int startCol = duRGBA(128, 25, 0, 192);
        duDebugDraw& dd = Wiwa::RecastManager::m_RecastMesh->getDebugDraw();

        dd.depthMask(false);

        // Agent dimensions.	
        duDebugDrawCylinderWire(&dd, m_CurrentPos.x - m_AgentParams.radius, m_CurrentPos.y + 0.02f, m_CurrentPos.z - m_AgentParams.radius, m_CurrentPos.x + m_AgentParams.radius, m_CurrentPos.y + m_AgentParams.height, m_CurrentPos.z + m_AgentParams.radius, startCol, 2.0f);

        duDebugDrawCircle(&dd, m_CurrentPos.x, m_CurrentPos.y + 1, m_CurrentPos.z, m_AgentParams.radius, duRGBA(0, 0, 0, 64), 1.0f);

        unsigned int colb = duRGBA(0, 0, 0, 196);
        dd.begin(DU_DRAW_LINES);
        dd.vertex(m_CurrentPos.x, m_CurrentPos.y - 1, m_CurrentPos.z, colb);
        dd.vertex(m_CurrentPos.x, m_CurrentPos.y + 1, m_CurrentPos.z, colb);
        dd.vertex(m_CurrentPos.x - m_AgentParams.radius / 2, m_CurrentPos.y + 0.02f, m_CurrentPos.z, colb);
        dd.vertex(m_CurrentPos.x + m_AgentParams.radius / 2, m_CurrentPos.y + 0.02f, m_CurrentPos.z, colb);
        dd.vertex(m_CurrentPos.x, m_CurrentPos.y + 0.02f, m_CurrentPos.z - m_AgentParams.radius / 2, colb);
        dd.vertex(m_CurrentPos.x, m_CurrentPos.y + 0.02f, m_CurrentPos.z + m_AgentParams.radius / 2, colb);
        dd.end();
        dd.depthMask(true);
        
    }
    void NavAgentSystem::DrawPath()
    {
        const unsigned int startCol = duRGBA(128, 25, 0, 192);
        duDebugDraw& dd = Wiwa::RecastManager::m_RecastMesh->getDebugDraw();

        if (m_AgentIndex != -1) {
            const dtCrowdAgent* agent = Crowd::getInstance().getCrowd().getAgent(m_AgentIndex);
            if (agent)
            {
                dd.depthMask(false);
                const unsigned int spathCol = duRGBA(64, 16, 0, 220);
                /*const unsigned int offMeshCol = duRGBA(128, 96, 0, 220);*/
                dd.begin(DU_DRAW_LINES, 2.0f);
                for (int i = 0; i < agent->ncorners - 1; ++i)
                {
                    dd.vertex(agent->cornerVerts[i * 3], agent->cornerVerts[i * 3 + 1] + 0.4f, agent->cornerVerts[i * 3 + 2], spathCol);
                    dd.vertex(agent->cornerVerts[(i + 1) * 3], agent->cornerVerts[(i + 1) * 3 + 1] + 0.4f, agent->cornerVerts[(i + 1) * 3 + 2], spathCol);
                }
                dd.end();
            }
        }
        dd.depthMask(true);

    }
}

