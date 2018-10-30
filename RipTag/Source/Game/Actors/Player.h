#pragma once
#include <Multiplayer.h>
#include "Actor.h"
#include "EngineSource/3D Engine/Components/Base/CameraHolder.h"
#include "../../Physics/Wrapper/PhysicsComponent.h"
#include <functional>
#include "../../Input/Input.h"
#include "../Abilities/TeleportAbility.h"
#include "../Abilities/PossessGuard.h"
#include "../Abilities/BlinkAbility.h"
#include "2D Engine/Quad/Components/HUDComponent.h"
#include "../Abilities/VisabilityAbility.h"
#include "Enemy/Enemy.h"
#include "../Abilities/Disable/DisableAbility.h"


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
	bool possess = false;
	bool unlockMouse = false;
	bool pickup = false;
};



//This value has to be changed to match the players 
class Player : public Actor, public CameraHolder, public PhysicsComponent , public HUDComponent
{
private:
	const DirectX::XMFLOAT4A DEFAULT_UP{ 0.0f, 1.0f, 0.0f, 0.0f };
	const float MOVE_SPEED = 10.0f;
	const float SPRINT_MULT = 2.0f;
	const float JUMP_POWER = 400.0f;

	const unsigned short int m_nrOfAbilitys = 4;
private:
	//DisableAbility m_disable;
	AbilityComponent ** m_abilityComponents;	
	int m_currentAbility = 0;
	PossessGuard m_possess;
	BlinkAbility m_blink;
	float m_standHeight;
	float m_moveSpeed = 2.0f;
	float m_cameraSpeed = 1.0f;
	KeyPressed m_kp;

	float m_visability = 0.0f;

	bool m_lockPlayerInput;
	RayCastListener* m_rayListener;

	int mouseX = 0;
	int mouseY = 0;

	//Mana, if you want %. go currentMana
	float m_currentMana;
	float m_maxMana;

	const int STANDARD_START_MANA = 100;

	Quad * m_manaBar;
	
public:
	//Magic number
	static const int g_fullVisability = 2800;


	bool unlockMouse = false;
	Player();
	~Player();

	void Init(b3World& world, b3BodyType bodyType, float x, float y, float z);

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
	bool IsInputLocked();
	void UnlockPlayerInput();

	int getPossessState();
	const float & getVisability() const;
	const int & getFullVisability() const;

	
	//This is a way of checking if we can use the ability with out current mana
	bool CheckManaCost(const int & manaCost);

	bool DrainMana(const int & manaCost);
	void RefillMana(const int & manaFill);
private:
	void _handleInput(double deltaTime);
	void _onMovement();
	void _onSprint();
	void _onCrouch();
	void _onBlink();
	void _onPossess();
	void _onRotate(double deltaTime);
	void _onJump();
	void _onPickup();
	void _cameraPlacement(double deltaTime);
};
