#include <Wiwa.h>

#include "game/GameLayer.h"

USE_REFLECTION;
class WiwaApp : public Wiwa::Application
{
public:
	REFLECTION_BODY_INL;

	WiwaApp(int argc, char** argv) : Wiwa::Application(argc, argv)
	{
		REFLECTION_REGISTER();

		PushOverlay(new GameLayer());
	}

	~WiwaApp()
	{

	}
};

Wiwa::Application* Wiwa::CreateApplication(int argc, char** argv)
{
	return new WiwaApp(argc, argv);
}