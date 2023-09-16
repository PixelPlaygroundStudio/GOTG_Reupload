#pragma once

#include <Wiwa/core/Layer.h>
#include <Wiwa/Events/Event.h>
#include <Wiwa/Events/ApplicationEvent.h>

#define WI_DEBUG_BUILD

#ifdef WI_DEBUG_BUILD
	#include "windows.h"
	#include "psapi.h"
	#include <Wiwa/utilities/AllocationMetrics.h>
	#include <Wiwa/ecs/EntityManager.h>
	#include <Wiwa/ecs/components/game/Character.h>
#endif

class GameLayer : public Wiwa::Layer {
public:
	GameLayer();
	~GameLayer();

	void OnAttach() override;

	void OnDetach() override;

	void OnUpdate() override;

	void OnImGuiRender() override;

	void OnEvent(Wiwa::Event& e) override;

	bool OnGameSave(Wiwa::OnSaveEvent& e);

	bool OnGameLoad(Wiwa::OnLoadEvent& e);

private:
	

};