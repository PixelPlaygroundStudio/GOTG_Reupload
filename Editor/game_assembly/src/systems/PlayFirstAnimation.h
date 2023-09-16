#pragma once

#include <Wiwa/ecs/systems/System.h>

class PlayFirstAnimation : public Wiwa::System
{
public:
	void OnInit() override;

};

REGISTER_SYSTEM(PlayFirstAnimation);