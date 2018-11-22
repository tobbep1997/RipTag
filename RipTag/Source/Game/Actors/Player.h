#pragma once
#include <Multiplayer.h>
#include <functional>
#include <AudioEngine.h>
#include <string>
#include "Actor.h"
#include "EngineSource/3D Engine/Components/Base/CameraHolder.h"
#include "../../Physics/Wrapper/PhysicsComponent.h"
#include "2D Engine/Quad/Components/HUDComponent.h"
#include <stack>

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

enum Ability;
enum PlayerState;


class AbilityComponent;
class Enemy;
class BlinkAbility;
class PossessGuard;
class TeleportAbility;

//This value has to be changed to match the players 
class Player : public Actor, public CameraHolder, public PhysicsComponent, public HUDComponent
{
private: //stuff for state machine
	friend class PlayState;
	friend class Enemy; //#todoREMOVE
	bool m_jumpedThisFrame = false;
	bool m_isInAir = false;
	float m_currentSpeed = 0.0f; //[0,1]
	float m_currentDirection = 0.0; //[-1,1]
	static float m_currentPitch;
private:
	const DirectX::XMFLOAT4A DEFAULT_UP{ 0.0f, 1.0f, 0.0f, 0.0f };
	const float MOVE_SPEED = 5.0f;
	const float SPRINT_MULT = 2.0f;
	const float JUMP_POWER = 900.0f;
	const float INTERACT_RANGE = 3.0f;

	const unsigned short int m_nrOfAbilitys = 2;
	AudioEngine::Listener m_FMODlistener;
private:
	struct QuadPair
	{
		Quad * bckg = nullptr;
		Quad * forg = nullptr;
		void Draw()
		{
			if (bckg)
				bckg->Draw();
			if (forg)
				forg->Draw();
		}
		~QuadPair()
		{
			if (bckg)
			{
				bckg->Release();
				delete bckg;
				bckg = nullptr;
			}
			if (forg)
			{
				forg->Release();
				delete forg;
				forg = nullptr;
			}
		}
	};

	//DisableAbility m_disable;
	AbilityComponent ** m_abilityComponents1;
	AbilityComponent ** m_abilityComponents2;
	AbilityComponent ** m_activeSet;
	unsigned int m_activeSetID = 1;
	Ability m_currentAbility;// = Ability::TELEPORT;

	Circle ** m_abilityCircle;

	PlayerState m_currentState = PlayerState::Idle;
	Enemy* possessTarget;

	float m_moveSpeed = 4.0f;
	float m_scrollMoveModifier = 0.9f;
	float m_cameraSpeed = 1.0f;
	float m_currentMoveSpeed = 0.0f;
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
	float m_soundPercentage = 0.0f;

	bool m_lockPlayerInput;

	float m_objectInfoTime = 0.0f;

	int mouseX = 0;
	int mouseY = 0;

	Quad * m_infoText;
	Quad * m_abilityTutorialText;
	Quad * m_tutorialText;
	
	QuadPair m_soundLevelHUD;
	

	Quad * m_cross;
	std::stack<std::string> m_tutorialMessages;
	float m_tutorialDuration = 0.0f;
	bool m_tutorialActive = true;
	
	DirectX::XMVECTOR m_VlastSpeed; 
	DirectX::XMVECTOR m_VcurrentSpeed; 

	//Crouch
	float m_standHeight;
	float m_crouchHeight;
	int crouchDir = 0;
	//Peek
	int peekDir = 0;
	int LastPeekDir = 0;
	float m_peekRotate;
	float m_peekRangeA = 0;
	float m_peekRangeB = 0;
	float m_peektimer = 0;
	bool  m_allowPeek = true;
	bool m_recentHeadCollision = false;

	Circle * m_HUDcircle;
	Circle * m_HUDcircleFiller;

	const unsigned short MAX_ENEMY_CIRCLES = 10;
	std::vector<Circle*> m_enemyCircles;
	float totVis = 0;
	float maxVis = 0;
	unsigned short m_currentEnemysVisable = 0;

	bool m_MapPicked = false;
	unsigned int m_rockCounter = 0;
	const unsigned int MAXROCKS = 5;
public:
	//Magic number
	static const int g_fullVisability = 1300;

	bool hasWon = false;
	
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
	void SendAbilityUpdates();
	void SendOnAnimationUpdate(double dt);
	void SendOnWinState();
	void RegisterThisInstanceToNetwork();

	void SetCurrentVisability(const float & guard);
	void SetCurrentSoundPercentage(const float & percentage);

	void LockPlayerInput();
	bool IsInputLocked();
	void UnlockPlayerInput();

	const float & getVisability() const;
	const int & getFullVisability() const;
	const bool & getWinState() const { return hasWon; }

	const AudioEngine::Listener & getFMODListener() const; 
	
	//This is a way of checking if we can use the ability with out current mana
	void SetAbilitySet(int set);

	void setEnemyPositions(std::vector<Enemy *> enemys);

	const Ability getCurrentAbility()const;
	TeleportAbility * getTeleportAbility();
	unsigned int getNrOfRocks();
	bool GetMapPicked();
private:
	void _collision();
	void _handleInput(double deltaTime);
	void _onMovement(double deltaTime);
	void _scrollMovementMod();
	void _onSprint();
	void _onCrouch();
	void _onRotate(double deltaTime);
	//void _onJump();
	void _onPeak(double deltaTime);
	void _onInteract();
	void _onAbility(double dt);
	void _objectInfo(double deltaTime);
	void _updateTutorial(double deltaTime);

	void _cameraPlacement(double deltaTime);
	void _updateFMODListener(double deltaTime, const DirectX::XMFLOAT4A & xmLastPos);
	void _activateCrouch(); 
	void _deActivateCrouch();
	void _hasWon();
	b3Vec3 _slerp(b3Vec3 start, b3Vec3 end, float percent);

	//Cheats, like changing ability set
	void _cheats();
};
