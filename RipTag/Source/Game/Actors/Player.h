#pragma once
#include <Multiplayer.h>
#include "Actor.h"
#include "EngineSource/3D Engine/Components/Base/CameraHolder.h"
#include "../../Physics/Wrapper/PhysicsComponent.h"
#include <functional>
#include "../../Input/Input.h"
#include "../../Physics/Wrapper/RayCastListener.h"
#include "../Abilities/TeleportAbility.h"
#include "Enemy/Enemy.h"

namespace FUNCTION_STRINGS
{
	static const char * JUMP = "Jump";
}

struct KeyPressed
{
	bool jump = false;
	bool crouching = false;
	bool teleport = false;
	bool blink = false;
	bool unlockMouse = false;
};

class Player : public Actor, public CameraHolder, public PhysicsComponent
{
private:
	const DirectX::XMFLOAT4A DEFAULT_UP{ 0.0f, 1.0f, 0.0f, 0.0f };
	const float MOVE_SPEED = 10.0f;
	const float SPRINT_MULT = 2.0f;
	const float JUMP_POWER = 400.0f;

private:
	Enemy* possessTarget;
	TeleportAbility m_teleport;
	float m_standHeight;
	RayCastListener *m_rayListener;
	float m_moveSpeed = 2.0f;
	float m_cameraSpeed = 1.0f;
	KeyPressed m_kp;

	float m_visability = 0.0f;

	bool m_lockPlayerInput;

	Drawable * visSphear;

	int mouseX = 0;
	int mouseY = 0;
public:
	bool unlockMouse = false;
	Player();
	~Player();

	void BeginPlay();
	void Update(double deltaTime);

	void PhysicsUpdate(double deltaTime);

	void setPosition(const float& x, const float& y, const float& z, const float& w = 1.0f) override;

	void Draw() override;

	//Networking
	void SendOnJumpMessage();
	void SendOnMovementMessage();

	void RegisterThisInstanceToNetwork();

	void SetCurrentVisability(const float & guard);

	void LockPlayerInput();
	void UnlockPlayerInput();

	void Phase(float searchLength);
	void possessGuard(float searchLength);
	Enemy* getPossesTarget() { return this->possessTarget; };
private:
	void _handleInput(double deltaTime);
	void _onMovement();
	void _onSprint();
	void _onCrouch();
	void _onBlink();
	void _onRotate(double deltaTime);
	void _onJump();
	void _onCheckVisibility();
	void _onTeleport(double deltaTime);
	void _cameraPlacement(double deltaTime);
};
