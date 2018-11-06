#pragma once

#include <AudioEngine.h>
#include <Multiplayer.h>

#include <string>
#include <vector>
#include <iostream>
#include <experimental/filesystem>
#include <algorithm>
#include <bits.h>

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
#include "Source/Gameplay/Triggers/Triggerable.h"
#include "Source/Gameplay/Triggers/TriggerHandler.h"
#include "Source/Gameplay/Objects/Door.h"
#include "Source/Gameplay/Objects/Bars.h"
#include "Source/Gameplay/Objects/Lever.h"
#include "Source/Gameplay/Objects/PressurePlate.h"

#include "Source/Physics/Bounce.h"
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

//Engien
#include "EngineSource/3D Engine/Extern.h"
#include "EngineSource/3D Engine/RenderingManager.h"
#include "EngineSource/3D Engine/Components/Camera.h"
#include "EngineSource/3D Engine/Components/Base/Drawable.h"
#include "EngineSource/3D Engine/Components/StateMachine.h"
#include "EngineSource/3D Engine/Model/Meshes/AnimatedModel.h"
#include "EngineSource/3D Engine/Model/Managers/MeshManager.h"
#include "EngineSource/3D Engine/Model/Managers/TextureManager.h"
#include "EngineSource/3D Engine/Model/Managers/MeshManager.h"


#include "EngineSource/Helper/Timer.h"
#include "EngineSource/Helper/HelperFunctions.h"
#include "EngineSource/Light/PointLight.h"
#include "EngineSource/Debugg/ImGui/imgui.h"
#include "EngineSource/Debugg/ImGui/ImGuiManager.h"


//Input
#include "InputManager/InputHandler.h"
#include "InputManager/XboxInput/GamePadHandler.h"
//Import
#include "ImportLibrary/formatImporter.h"
#include "ImportLibrary/FormatHeader.h"

//2D
#include "2D Engine/Quad/Quad.h"
#include "2D Engine/DirectXTK/SpriteFont.h"
