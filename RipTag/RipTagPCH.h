#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <experimental/filesystem>

#include "RipTagExtern/RipExtern.h"

#include "Source/Game/States/State.h"
#include "Source/Game/States/MainMenu.h"
#include "Source/Game/States/PlayState.h"

#include "Source/Input/Input.h"
#include "Source/Game/Level/Room.h"
#include "Source/Game/Actors/Player.h"
#include "Source/Game/Actors/BaseActor.h"
#include "Source/Game/Actors/RemotePlayer.h"
#include "Source/Game/Actors/Enemy/Enemy.h"
#include "Source/Game/Actors/Assets/StaticAsset.h"
#include "Source/Game/Pathfinding/Tile.h"
#include "Source/Game/Pathfinding/Grid.h"
#include "Source/Game/Handlers/CameraHandler.h"
#include "Source/Game/Handlers/AnimationHandler.h"
#include "Source/Game/Handlers/LevelHandler.h"
#include "Source/Game/Handlers/PlayerManager.h"

#include "Source/Gameplay/Triggers/Trigger.h"
#include "Source/Gameplay/Triggers/Triggerble.h"
#include "Source/Gameplay/Triggers/TriggerHandler.h"
#include "Source/Gameplay/Objects/Door.h"
#include "Source/Gameplay/Objects/Lever.h"
#include "Source/Gameplay/Objects/PressurePlate.h"

#include "Source/Physics/Wrapper/CollisionBoxes.h"
#include "Source/Physics/Wrapper/RayCastListener.h"
#include "Source/Physics/Wrapper/ContactListener.h"
#include "Source/Physics/Wrapper/PhysicsComponent.h"

#include "Source/Game/Abilities/Disable/DisableAbility.h"
#include "Source/Game/Abilities/AbilityComponent.h"
#include "Source/Game/Abilities/BlinkAbility.h"
#include "Source/Game/Abilities/PossessGuard.h"
#include "Source/Game/Abilities/TeleportAbility.h"
#include "Source/Game/Abilities/VisabilityAbility.h"

#include "Source/Timer/DeltaTime.h"