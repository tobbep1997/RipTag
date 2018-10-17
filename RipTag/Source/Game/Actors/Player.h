#pragma once
#include "Actor.h"
#include "Abilities/Teleport.h"
#include "EngineSource/3D Engine/Components/Base/CameraHolder.h"
#include "../../Physics/Wrapper/PhysicsComponent.h"
#include <functional>
#include "../../Input/Input.h"

namespace FUNCTION_STRINGS
{
	static const char * JUMP = "Jump";

}


class Player : public Actor, public CameraHolder, public PhysicsComponent
{
private:
	const DirectX::XMFLOAT4A MAX_PEEK_LEFT{-0.5f, 0.5f, 0.0f, 0.0f};
	const DirectX::XMFLOAT4A MAX_PEEK_RIGHT{ 0.5f, 0.5f, 0.0f, 0.0f };
	const DirectX::XMFLOAT4A DEFAULT_UP{ 0.0f, 1.0f, 0.0f, 0.0f };

private:
	
	Teleport m_teleport;

	float deltaTime = 0.0f;

	float m_moveSpeed = 100.0f;
	float m_cameraSpeed = 1.0f;
	bool isPressed = false;
	bool isPressed2 = false;
	float walkBob = 0.0f;
	float m_offset = 0.0f;
	float freq = 1.9f;
	float walkingBobAmp = 0.06f;
	float stopBobAmp = 0.010f;
	float stopBobFreq = 1.9f;
	float m_currentAmp = 0.0f;
	DirectX::XMFLOAT4A m_lastPeek;
	float m_peekSpeed = 2.0f;

	//STUFF RELATED TO STATE MACHINE HANDLING
	bool hasJumped = false;
	bool isRising = false;
	bool isFalling = false;

public:
	Player();
	~Player();

	void BeginPlay();
	void Update(double deltaTime);

	void PhysicsUpdate(double deltaTime);

	void setPosition(const float& x, const float& y, const float& z, const float& w) override;

	void InitTeleport(b3World & world);
	void ReleaseTeleport(b3World & world);

	void Draw() override;

	//GAMEPLAY FUNCTIONS
	void Jump(); //implement logic
	void MoveRight(); //implement logic
	void MoveLeft(); //implement logic
	void MoveForward(); //implement logic
	void MoveBackward(); //implement logic

	void RegisterThisInstanceToInput();
	void RegisterThisInstanceToNetwork();
private:

	void _handleInput(double deltaTime);
	void _moveDirection(bool);
	void _moveRight(bool);

};
