#pragma once
#include <Multiplayer.h>
#include "Actor.h"
#include "EngineSource/3D Engine/Components/Base/CameraHolder.h"
#include "EngineSource/3D Engine/Components/StateMachine.h"
#include "../../Physics/Wrapper/PhysicsComponent.h"
#include <functional>
#include "../../Input/Input.h"
#include "../Abilities/TeleportAbility.h"
#include "../Abilities/PossessGuard.h"
#include "../Abilities/BlinkAbility.h"
#include "2D Engine/Quad/Components/HUDComponent.h"
#include <AudioEngine.h>
#include "../Abilities/VisabilityAbility.h"
#include "Enemy/Enemy.h"
#include "../Abilities/Disable/DisableAbility.h"
#include <string>


struct KeyPressed
{
	bool jump = false;
	bool crouching = false;
	bool teleport = false;
	bool blink = false;
	bool possess = false;
	bool unlockMouse = false;
	bool interact = false;
};



//This value has to be changed to match the players 
class Player : public Actor, public CameraHolder, public PhysicsComponent , public HUDComponent
{
private: //stuff for state machine
	friend class PlayState;
	bool m_jumpedThisFrame = false;
	bool m_isInAir = false;
	float m_currentSpeed = 0.0f; //[0,1]
	float m_currentDirection = 0.0; //[-1,1]

	std::vector<std::string> m_sounds;


private:
	const DirectX::XMFLOAT4A DEFAULT_UP{ 0.0f, 1.0f, 0.0f, 0.0f };
	const float MOVE_SPEED = 4.0f;
	const float SPRINT_MULT = 2.0f;
	const float JUMP_POWER = 900.0f;
	const float INTERACT_RANGE = 2.0f;

	const unsigned short int m_nrOfAbilitys = 4;
	AudioEngine::Listener m_FMODlistener;
private:
	//DisableAbility m_disable;
	AbilityComponent ** m_abilityComponents;	
	Ability m_currentAbility = Ability::TELEPORT;

	Enemy* possessTarget;	
	PlayerState m_currentState = PlayerState::Idle;
	BlinkAbility m_blink;
	PossessGuard m_possess;
	float m_standHeight;
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

	int mouseX = 0;
	int mouseY = 0;

	//Mana, if you want %. go currentMana
	float m_currentMana;
	float m_maxMana;

	const int STANDARD_START_MANA = 100;
	Quad * m_manaBar;


	
public:
	//Magic number
	static const int g_fullVisability = 2300;


	bool unlockMouse = false;
	Player();
	Player(RakNet::NetworkID nID, float x, float y, float z);

	~Player();

	void Init(b3World& world, b3BodyType bodyType, float x, float y, float z);

	void BeginPlay();
	void Update(double deltaTime);

	void PhysicsUpdate();

	void setPosition(const float& x, const float& y, const float& z, const float& w = 1.0f) override;

	void Draw() override;

	//Networking
	void SendOnUpdateMessage();
	void SendOnAbilityUsed();
	void RegisterThisInstanceToNetwork();

	void SetCurrentVisability(const float & guard);

	void LockPlayerInput();
	bool IsInputLocked();
	void UnlockPlayerInput();

	const float & getVisability() const;
	const int & getFullVisability() const;

	const AudioEngine::Listener & getFMODListener() const; 
	
	//This is a way of checking if we can use the ability with out current mana
	bool CheckManaCost(const int & manaCost);

	bool DrainMana(const float & manaCost);
	void RefillMana(const float & manaFill);
private:
	void _handleInput(double deltaTime);
	void _onMovement();
	void _onSprint();
	void _onCrouch();
	void _onBlink();
	void _onPossess();
	void _onRotate(double deltaTime);
	void _onJump();
	void _onInteract();
	void _onAbility(double dt);


	void _cameraPlacement(double deltaTime);
	void _updateFMODListener(double deltaTime, const DirectX::XMFLOAT4A & xmLastPos);
	void _activateCrouch(); 
	void _deActivateCrouch();
};
