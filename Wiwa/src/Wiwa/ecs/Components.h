#pragma once

// Transform component
#include "components/Transform3D.h"

// Mesh component
#include "components/Mesh.h"

// Audio components
#include "components/AudioSource.h"
#include "components/AudioListener.h"

// Light components
#include "components/DirectionalLight.h"
#include "components/PointLight.h"
#include "components/SpotLight.h"

// Animations
//#include "components/AnimatorComponent.h"
#include "components/OzzAnimatorCmp.h"

// Physics components
#include "components/CollisionBody.h"
#include "components/ColliderCube.h"
#include "components/ColliderSphere.h"
#include "components/ColliderCylinder.h"
#include "components/RayCast.h"


// Game components
#include "components/game/Character.h"
#include "components/game/wave/Wave.h"
#include "components/game/wave/WaveSpawner.h"
#include "components/game/wave/WaveSpawnPoint.h"
#include "components/game/enemy/Enemy.h"
#include "components/game/wave/Wave.h"
#include "components/game/Health.h"
#include "components/game/items/Item.h"

#include "components/game/enemy/PhalanxMelee.h"
#include "components/game/enemy/PhalanxRanged.h"
#include "components/game/enemy/Sentinel.h"
#include "components/game/enemy/Subjugator.h"
#include "components/game/enemy/Ultron.h"

#include "components/HitComponent.h"

// Ai components
#include "components/MapAI.h"
#include "components/ai/NavMesh.h"
#include "components/ai/NavAgent.h"

#include "components/game/player/PlayerSpawnerData.h"