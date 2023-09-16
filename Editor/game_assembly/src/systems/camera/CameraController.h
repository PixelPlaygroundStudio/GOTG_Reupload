#pragma once
#include <Wiwa/core/Core.h>
#include <Wiwa/ecs/systems/System.h>
#include <glm/glm.hpp>

namespace Wiwa
{
	class  CameraController : public System
	{
	
	public:
		CameraController();
		~CameraController();

		void OnAwake() override;

		void OnInit() override;

		void OnUpdate() override;


		void OnDestroy() override;

    private:
        void InitCameraAgents();
        void InitCamera();
        void InitFOV();
        void BattleRoomFinished();
        glm::vec3 CalculateForward(Transform3D transform)
        {
            glm::vec3 rotrad;
            rotrad.x = DegToRad(transform.localRotation.x);
            rotrad.y = DegToRad(transform.localRotation.y);
            rotrad.z = DegToRad(transform.localRotation.z);

            glm::vec3 forward;
            forward.x = cos(rotrad.x) * sin(rotrad.y);
            forward.y = -sin(rotrad.x);
            forward.z = cos(rotrad.x) * cos(rotrad.y);

            forward.x = RadToDeg(forward.x);
            forward.y = RadToDeg(forward.y);
            forward.z = RadToDeg(forward.z);

            return glm::normalize(forward);
        }

        float DegToRad(float deg)
        {
            return deg * PI / 180.0f;
        }

        float RadToDeg(float rad)
        {
            return rad * 180.0f / PI;
        }
        float mapValue(float value, float input_min, float input_max, float output_min, float output_max)
        {
            // Normalize the value between 0 and 1
            float normalized_value = (value - input_min) / (input_max - input_min);

            // Map the normalized value to the output range
            float mapped_value = output_min + (normalized_value * (output_max - output_min));

            return mapped_value;
        }
        void UpdateCameraAgentsFarToNear();
        void UpdateCameraAgentsNearToFar(float minFOV, float maxFOV);
    private:
        Camera* m_Camera;
        EntityId m_PlayerId;
        EntityManager::ComponentIterator m_PlayerTransformIt;
        EntityManager::ComponentIterator m_TransformIt;
        EntityManager::ComponentIterator m_CameraDataIt;


        std::vector<struct CameraAgent*> m_CameraAgents;

        glm::vec3 m_CameraOffset{ 0.f, 40.0f, -15.0f };
        glm::vec3 m_CameraRotation{ 90.f, -65.f, 0.f };
        float m_FOV = 60.f;
        float m_Velocity = 2.f;
        float m_TargetFOV = 60.f;
        
	};
}
REGISTER_SYSTEM(Wiwa::CameraController);

