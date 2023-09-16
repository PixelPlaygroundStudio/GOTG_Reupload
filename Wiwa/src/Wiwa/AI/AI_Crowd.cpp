#include <wipch.h>
#include "AI_Crowd.h"

Crowd::Crowd() : m_Crowd(nullptr)
{
    Init();
}

Crowd::~Crowd()
{
    CleanUp();
}

void Crowd::Init()
{
    CleanUp();

    // Initialize crowd simulation
    m_Crowd = dtAllocCrowd();
    m_Crowd->init(MAX_AGENTS, 0.6f, Wiwa::RecastManager::getNavMesh());
    memset(&m_AgentParams, 0, sizeof(m_AgentParams));
    m_AgentParams.radius = 0.6f;
    m_AgentParams.height = 1.8f;
    m_AgentParams.maxAcceleration = 8.0f;
    m_AgentParams.maxSpeed = 3.0f;
    m_AgentParams.collisionQueryRange = m_AgentParams.radius * 12.0f;
    m_AgentParams.pathOptimizationRange = m_AgentParams.radius * 30.0f;
    m_AgentParams.updateFlags = DT_CROWD_ANTICIPATE_TURNS | DT_CROWD_OPTIMIZE_VIS | DT_CROWD_SEPARATION;
    m_AgentParams.separationWeight = 20.f;
}

void Crowd::CleanUp()
{
    dtFreeCrowd(m_Crowd);
    m_Crowd = 0;
}


void Crowd::Update(float deltaTime)
{
    OPTICK_EVENT("Crowd Update")
    m_Crowd->update(deltaTime, nullptr);
}

int Crowd::AddAgent(const float* position, dtCrowdAgentParams* param, bool defaultParam)
{
    if (!param)
        return -1;

    if (defaultParam){
        *param = m_AgentParams;
    }
    int index = m_Crowd->addAgent(position, param);
    if (index != -1) {
        m_Agents.emplace_back(index);
        param->userData = &m_Agents.back();
    }
    return index;
}

void Crowd::RemoveAgent(int agentIndex)
{
    if (agentIndex >= 0) {
        m_Crowd->removeAgent(agentIndex);
        std::vector<int>::iterator it = std::find(m_Agents.begin(), m_Agents.end(), agentIndex);
        if (it != m_Agents.end()) {
            m_Agents.erase(it);
        }
        //m_Agents.erase(m_Agents.begin() + agentIndex);
    }
}

bool Crowd::SetAgentTarget(int agentIndex, const float* target)
{
    if (agentIndex >= 0) {
        const dtCrowdAgent* agent = m_Crowd->getAgent(agentIndex);

        dtPolyRef targetRef;
        float targetPos[3];
        const dtQueryFilter* filter = m_Crowd->getFilter(agentIndex);
        const float* extents = m_Crowd->getQueryExtents();
        m_Crowd->getNavMeshQuery()->findNearestPoly(target, extents, filter, &targetRef, targetPos);
        bool isSetCorrect = m_Crowd->requestMoveTarget(agentIndex, targetRef, targetPos);
        return isSetCorrect;
    }
    return false;
}

void Crowd::SetAgentMaxSpeed(int agentIndex, float speed) {
    if (agentIndex >= 0) {
        dtCrowdAgentParams params = m_AgentParams;
        params.maxSpeed = speed;
        m_Crowd->updateAgentParameters(agentIndex, &params);
    }
}

void Crowd::SetAgentMaxAcceleration(int agentIndex, float acceleration)
{
    if (agentIndex >= 0) {
        dtCrowdAgentParams params = m_AgentParams;
        params.maxAcceleration = acceleration;
        m_Crowd->updateAgentParameters(agentIndex, &params);
    }
}

void Crowd::SetAgentParameters(int agentIndex, const dtCrowdAgentParams& params)
{
    m_Crowd->updateAgentParameters(agentIndex, &params);
}

int Crowd::GetAgentCount() const
{
    if (m_Agents.size() == m_Crowd->getAgentCount())
        return  m_Crowd->getAgentCount();
    else
        return -1;
}

const float* Crowd::GetAgentPosition(int agentIndex) const
{
    if (agentIndex >= 0) {
        const dtCrowdAgent* agent = m_Crowd->getAgent(agentIndex);
        return agent->npos;
    }
    return nullptr;
}

const float* Crowd::GetAgentVelocity(int agentIndex) const
{
    if (agentIndex >= 0) {
        const dtCrowdAgent* agent = m_Crowd->getAgent(agentIndex);
        return agent->dvel;
    }
    return nullptr;
}

glm::vec3 Crowd::GetAgentPositionVec(int agentIndex) const
{
    if (agentIndex >= 0) {
        const dtCrowdAgent* agent = m_Crowd->getAgent(agentIndex);
        return glm::vec3(agent->npos[0], agent->npos[1], agent->npos[2]);
    }
    return glm::vec3(0.0f);
}

glm::vec3 Crowd::GetAgentVelocityVec(int agentIndex) const
{
    if (agentIndex >= 0) {
        const dtCrowdAgent* agent = m_Crowd->getAgent(agentIndex);
        return glm::vec3(agent->dvel[0], agent->dvel[1], agent->dvel[2]);
    }
    return glm::vec3(0.0f);
}



