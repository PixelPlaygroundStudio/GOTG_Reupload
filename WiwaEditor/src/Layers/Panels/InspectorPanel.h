#pragma once

#include "Panel.h"

#include <Wiwa/utilities/Reflection.h>
#include <Wiwa/utilities/containers/Array.h>

#include <Wiwa/utilities/math/Vector3f.h>
#include <Wiwa/ecs/EntityManager.h>
#include <Wiwa/events/ApplicationEvent.h>

class InspectorPanel : public Panel
{
private:
	bool DrawComponent(size_t componentId);
	void DrawField(unsigned char* data, const Field& field);

	void DrawCollisionTags();

	void DrawMeshComponent(byte* data);
	void DrawTransform3dComponent(byte* data);
	void DrawAudioSourceComponent(byte* data);
	void DrawPointLightComponent(byte* data);
	void DrawDirectionalLightComponent(byte* data);
	void DrawSpotLightComponent(byte* data);
	void DrawAnimatorComponent(byte* data);
	void DrawCollisionBodyComponent(byte* data);
	void DrawColliderCubeComponent(byte* data);
	void DrawColliderSpehereComponent(byte* data);
	void DrawColliderCylinderComponent(byte* data);
	void DrawRayCastComponent(byte* data);
	void DrawParticleSystemComponent(byte* data);
	void DrawItemComponent(byte* data);
	void DrawNavMeshComponent(byte* data);
	void DrawOzzAnimatorCmp(byte* data);
	void DrawPlayerSpawnerComponent(byte* data);
public:

	InspectorPanel(EditorLayer* instance);
	virtual ~InspectorPanel();

	void Draw() override;
	void DrawComponents(Wiwa::EntityManager& em);
	void DrawSystems(Wiwa::EntityManager& em);
	void Update() override;

	void OnEvent(Wiwa::Event&) override;
	bool OnEntityChangeEvent(EntityChangeEvent& e);
	bool OnSceneChangeEvent(Wiwa::SceneChangeEvent& e);

	inline bool GetCurrentEntity(int& id)
	{
		id = m_CurrentID;
		return s_EntitySet;
	}
private:
	
	int m_CurrentID = 0;
public:
	static bool s_EntitySet;
};
