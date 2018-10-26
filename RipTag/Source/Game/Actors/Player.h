#pragma once
#include <Multiplayer.h>
#include "Actor.h"
#include "Abilities/Teleport.h"
#include "EngineSource/3D Engine/Components/Base/CameraHolder.h"
#include "../../Physics/Wrapper/PhysicsComponent.h"
#include <functional>
#include "../../Input/Input.h"
#include "../../Physics/Wrapper/RayCastListener.h"

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
	const float MOVE_SPEED = 4.0f;
	const float SPRINT_MULT = 2.0f;
	const float JUMP_POWER = 2200.0f;

private:
	Teleport m_teleport;
	float m_standHeight;
	RayCastListener *m_rayListener;
	float m_moveSpeed = 4.0f;
	float m_cameraSpeed = 1.0f;
	float m_offPutY = 0.4f; 
	
	bool m_currClickCrouch = false; 
	bool m_prevClickCrouch = false;
	bool m_currClickSprint = false; 
	bool m_prevClickSprint = false; 
	bool m_isSprinting = false; 
	
	int m_toggleCrouch = 0; 
	int m_toggleSprint = 0; 

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

	void InitTeleport(b3World & world);
	void ReleaseTeleport(b3World & world);

	void Draw() override;

	//Networking
	void SendOnJumpMessage();
	void SendOnMovementMessage();

	void RegisterThisInstanceToNetwork();

	void SetCurrentVisability(const float & guard);

	void LockPlayerInput();
	void UnlockPlayerInput();

	void Phase(float searchLength);

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
	void _activateCrouch(); 
	void _deActivateCrouch();
};
