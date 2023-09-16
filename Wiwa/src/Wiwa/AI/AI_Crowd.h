#pragma once
#include <DetourCrowd.h>
#include <vector>
#include <glm/glm.hpp>
#include "AI_RecastManager.h"
#define MAX_AGENTS 60

class Crowd {
public:

    //static Crowd* instancePtr;
    Crowd(const Crowd& obj)
        = delete;
    
    static Crowd& getInstance()
    {
        static Crowd instance;
        return instance;
    }

    void Update(float deltaTime);
    int AddAgent(const float* position, dtCrowdAgentParams* param, bool defaultParam = false);
    void RemoveAgent(int agentIndex);
    bool SetAgentTarget(int agentIndex, const float* target);
    void SetAgentMaxSpeed(int agentIndex, float speed);
    void SetAgentMaxAcceleration(int agentIndex, float acceleration);
    void SetAgentParameters(int agentIndex, const dtCrowdAgentParams& param);
    int GetAgentCount() const;
    const float* GetAgentPosition(int agentIndex) const;
    const float* GetAgentVelocity(int agentIndex) const;
    glm::vec3 GetAgentPositionVec(int agentIndex) const;
    glm::vec3 GetAgentVelocityVec(int agentIndex) const;

    void Init();
    void CleanUp();

    dtCrowd& getCrowd() { return *m_Crowd; };
    
private:
    Crowd();
    ~Crowd();
    dtCrowd* m_Crowd;
    dtCrowdAgentParams m_AgentParams;
    std::vector<int> m_Agents;
};