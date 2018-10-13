#pragma once
#include "Actor.h"
#include "Abilities/Teleport.h"
#include "EngineSource/3D Engine/Components/Base/CameraHolder.h"
#include "../../Physics/Wrapper/PhysicsComponent.h"

class Player : public Actor, public CameraHolder, public PhysicsComponent
{
private:
	Teleport m_teleport;

	float m_moveSpeed = 200.0f;
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

private:

	void _handleInput(double deltaTime);

};
