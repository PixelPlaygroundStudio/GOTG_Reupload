#pragma once

#include "Panel.h"
#include "Wiwa/audio/Audio.h"
#include "Wiwa/Events/ApplicationEvent.h"

class AudioPanel : public Panel
{
public:
	AudioPanel(EditorLayer* instance);
	virtual ~AudioPanel();

	void Draw() override;

	void OnEvent(Wiwa::Event& e) override;
	bool OnSceneChange(Wiwa::SceneChangeEvent& e);

	bool loaded_project = false;
	std::string filename = "";
	bool ret = false;
	std::vector<Audio::BankData> bank_list;
	std::vector<Audio::EventData> event_list;
	size_t b_size = 0;
	size_t e_size = 0;
	uint32_t del_bank_id = 0;
	char buffer[64] = "";
	// Need to reorganize that 
};