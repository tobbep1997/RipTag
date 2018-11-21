#pragma once
//MULLE MECKA
#pragma warning (disable : 4215 4267 4305)
#include <AudioEngine.h>
#include <Multiplayer.h>

#include <string>
#include <vector>
#include <iostream>
#include <experimental/filesystem>
#include <algorithm>
#include <bits.h>

#include "RipTagExtern/RipExtern.h"

#include <DirectXCollision.h>
#include "Source/ParticleSystem/Particle.h"
#include "Source/ParticleSystem/ParticleEmitter.h"
#include "Source/ParticleSystem/ParticleManager.h"
#include "Source/ParticleSystem/ParticleSystem.h"

#include "Source/Game/States/State.h"
#include "Source/Game/States/MainMenu.h"
#include "Source/Game/States/PlayState.h"
#include "Source/Game/States/OptionState.h"
#include "Source/Game/States/LobbyState.h"
#include "Source/Game/States/OptionSound.h"
#include "Source/Game/States/LoseState.h"
#include "Source/Game/States/DBG/DBGState.h"

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
#include "Source/Game/Handlers/EnemyHandler.h"
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
//#include "Source/Game/Abilities/VisabilityAbility.h"

#include "Source/Timer/DeltaTime.h"

//Engien
#include "EngineSource/3D Engine/Extern.h"
#include "EngineSource/3D Engine/RenderingManager.h"
#include "EngineSource/3D Engine/Components/Camera.h"
#include "EngineSource/3D Engine/Components/Base/Drawable.h"
#include "EngineSource/3D Engine/Components/StateMachine.h"
#include "EngineSource/3D Engine/Components/LayerMachine.h"
#include "EngineSource/3D Engine/Model/Meshes/AnimationPlayer.h"
#include "EngineSource/3D Engine/Model/Managers/MeshManager.h"
#include "EngineSource/3D Engine/Model/Managers/TextureManager.h"
#include "EngineSource/3D Engine/Model/Managers/MeshManager.h"

#include "ConsoleColor.h"

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
#include "2D Engine/Circle/Circle.h"
#include "2D Engine/TextInput/TextInput.h"
#include "2D Engine/DirectXTK/SpriteFont.h"
#include "Fonts/FontHandler.h"


//This allows to use new instead of new to create a pointer

/****************************************/
/*		Microsoft code Pogchamp			*/
/*		Dont use on Microsoft stuff		*/
/*		AKA no DirectX					*/
/****************************************/

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
#define DBG_NEW new
#endif

static void printVector(const DirectX::XMFLOAT4 & vec)
{
	std::cout << "X: " << vec.x << "\tY: " << vec.y << "\tZ: " << vec.z << "\tW: " << vec.w << std::endl;
}

static void printVector(const DirectX::XMFLOAT4A & f)
{
	printVector(DirectX::XMFLOAT4(f.x, f.y, f.z, f.w));
}
static void printVector(const DirectX::XMFLOAT3 & f)
{
	printVector(DirectX::XMFLOAT4(f.x, f.y, f.z, 0));
}

static DirectX::XMFLOAT4 getDirection(const DirectX::XMFLOAT4 & iFrom, const DirectX::XMFLOAT4 & iToo)
{
	using namespace DirectX;

	XMVECTOR f = XMLoadFloat4(&iFrom);
	XMVECTOR t = XMLoadFloat4(&iToo);

	XMVECTOR vDir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(t, f));
	XMFLOAT4 dir; XMStoreFloat4(&dir, vDir);
	return dir;
}
static DirectX::XMFLOAT4 getDirection(const DirectX::XMFLOAT4A & iFrom, const DirectX::XMFLOAT4A & iToo)
{
	return getDirection(DirectX::XMFLOAT4(iFrom.x, iFrom.y, iFrom.z, iFrom.w), DirectX::XMFLOAT4(iToo.x, iToo.y, iToo.z, iToo.w));
}


static float getLenght(const DirectX::XMFLOAT4 & vec)
{
	using namespace DirectX;

	XMVECTOR f = XMLoadFloat4(&vec);

	XMVECTOR vDir = DirectX::XMVector4Length(f);
	XMFLOAT4 dir; XMStoreFloat4(&dir, vDir);
	return dir.x;
}

static float getLenght(const DirectX::XMFLOAT4 & vec, const DirectX::XMFLOAT4 & vec2)
{
	using namespace DirectX;

	XMVECTOR v1 = XMLoadFloat4(&vec);
	XMVECTOR v2 = XMLoadFloat4(&vec2);

	XMVECTOR vDir = DirectX::XMVector4Length(DirectX::XMVectorSubtract(v1, v2));
	XMFLOAT4 dir; XMStoreFloat4(&dir, vDir);
	return dir.x;
}