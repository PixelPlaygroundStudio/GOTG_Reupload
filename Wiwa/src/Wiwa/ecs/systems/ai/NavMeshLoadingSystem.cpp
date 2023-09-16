#include <wipch.h>
#include "NavMeshLoadingSystem.h"
#include <Wiwa/AI/AI_RecastManager.h>
#include <Wiwa/AI/AI_Crowd.h>

namespace Wiwa
{
	NavMeshLoadingSystem::NavMeshLoadingSystem()
	{
		/*RecastManager::Load();*/
	}

	NavMeshLoadingSystem::~NavMeshLoadingSystem()
	{
	
	}

	void NavMeshLoadingSystem::OnSystemAdded()
	{		
		//Crowd& crowd = Crowd::getInstance();
		//crowd.Init();
	}

	void NavMeshLoadingSystem::OnSystemRemoved()
	{
		/*Crowd& crowd = Crowd::getInstance();
		crowd.CleanUp();*/
	}

	void NavMeshLoadingSystem::OnUpdate()
	{
		/*OPTICK_EVENT("Crowd Update")
		Crowd& crowd = Crowd::getInstance();
		crowd.Update(Wiwa::Time::GetDeltaTimeSeconds());*/
	}
}
