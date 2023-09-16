#include "systems/player/states/PlayerStateMachine.h"
#include "systems/player/PlayerSpawner.h"
#include "systems/camera/CameraController.h"
#include "systems/player/CharacterSelector.h"

#include "systems/ships/ship_main_menu.h"
#include "systems/ships/ShipRotator.h"

#include "systems/enemy/EnemySystem.h"

#include "systems/enemy/MeleePhalanx/EnemyMeleePhalanx.h"
#include "systems/enemy/RangedPhalanx/EnemyRangedPhalanx.h"
#include "systems/enemy/Sentinel/EnemySentinel.h"
#include "systems/enemy/Ultron/BossUltron.h"
#include "systems/enemy/Subjugator/EnemySubjugator.h"

#include "systems/attack/SimpleBulletSystem.h"
#include "systems/attack/RainProjectileSystem.h"
#include "systems/attack/SentinelExplosionSystem.h"

#include "systems/attack/VictoryShieldSystem.h"
#include "systems/attack/StarhawksBlastSystem.h"
#include "systems/attack/PhylasQuantumSwordSystem.h"
#include "systems/attack/GrootSeedsSystem.h"
#include "systems/attack/CaptainUniverseEnergySystem.h"
#include "systems/attack/MantisTelepathicThrustSystem.h"

#include "systems/camera/CameraController.h"

#include "systems/NpcSystem.h"
#include "systems/ForcedDialogSystem.h"
#include "systems/CharacterSensitiveDialogSystem.h"
#include "systems/OneTimeDialogSystem.h"

#include "systems/attack/ClusterBulletSystem.h"
#include "systems/attack/ZigZagSystem.h"

#include "systems/attack/UltronLaserBeamSystem.h"
#include "systems/attack/SecondDashSystem.h"
#include "systems/attack/UltronSmashExplosionSystem.h"
#include "systems/enemy/BossMovementPointsSystem.h"
#include "systems/attack/YondusFinSystem.h"

#include "systems/attack/ExplosiveBarrelSystem.h"
#include "systems/props/ExplosiveBarrelProp.h"
#include "systems/props/VentRotator.h"
#include "systems/props/RewardChest.h"

#include "systems/InitPool.h"

#include "systems/player/StarLordBullet.h"

#include "systems/triggers/StartRun.h"
#include "systems/triggers/EndRoom.h"
#include "systems/triggers/InitHub.h"

#include "systems/SkipScene.h"
#include "systems/SkipIntro.h"
#include "systems/Reset_HubSystem.h"
#include "systems/MainMenuUISystem.h"

#include "systems/TestingSystem.h"
#include "systems/attack/StarlordUltimateSystem.h"
#include "systems/attack/StarhawkBlastBulletSystem.h"

#include "systems/triggers/EndRewardRoom.h"
#include "systems/InitMusic.h"
#include "systems/HowardShopSystem.h"

#include "systems/camera/CameraAgentSystem.h"
#include "systems/gui/PopUpItem.h"

#include "systems/hub/TargetSystem.h"
#include "systems/triggers/RewardRoomDisplay.h"

#include "systems/PlayFirstAnimation.h"
#include "systems/ContinueLoading.h"
#include "systems/triggers/InitBossRoom.h"