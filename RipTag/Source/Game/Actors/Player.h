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
public:
	friend class PlayerManager;
	void SetThrowing(bool throwing);
private: //stuff for state machine
	friend class PlayState;
	friend class Enemy;

	std::function<bool()> m_IsMoving = [&]() {return m_currentMoveSpeed > 0.1f; };

	bool m_playAnimation = true; 
	bool m_IsThrowing = false;
	bool m_isInAir = false;
	bool m_headBobbingActive = true; 
	float m_currentSpeed = 0.0f; //[0,1]
	float m_currentPeek = 0.0f;
	float m_currentDirection = 0.0; //[-180,180], relative to movement
	float m_currentWorldDirection = 0.0f; //relative to world
	float m_currentTurnSpeed = 0.0f;
	float m_lastDirection = 0.0f;
	static float m_currentPitch;
private:
	const DirectX::XMFLOAT4A DEFAULT_UP{ 0.0f, 1.0f, 0.0f, 0.0f };
	const float MOVE_SPEED = 3.3f;
	const float SPRINT_MULT = 1.8f;
	const float JUMP_POWER = 900.0f;
	const float INTERACT_RANGE = 3.0f;
	const float OBJECT_INFO_RANGE = 20.0f;

	const std::string PlayerOneHUDPath = "../Assets/Player1HUD.txt";
	const std::string PlayerTwoHUDPath = "../Assets/Player2HUD.txt";

	const unsigned short int m_nrOfAbilitys = 2;
	AudioEngine::Listener m_FMODlistener;
	AudioEngine::SoundDesc m_footSteps;
	AudioEngine::SoundDesc m_smokeBomb; 
private:
	//First-person model
	BaseActor* m_FirstPersonModel{ nullptr };

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

	unsigned int m_prevClickCrouch = 0; 
	unsigned int m_prevSprintInputType = 0;
	
	bool m_toggleSprint = 0;
	bool m_toggleCrouch = 0; 
	
	bool m_exitPause = false; 
	

	KeyPressed m_kp;
	
	float m_visability = 0.0f;
	float m_soundPercentage = 0.0f;

	bool m_lockPlayerInput;

	float m_objectInfoTime = 0.0f;

	int mouseX = 0;
	int mouseY = 0;
	
	QuadPair m_soundLevelHUD;
	
	Quad * m_cross;
	
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

	bool m_controlLayoutShown = false;

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

	int m_headBobRayId = -100; //HeadBob
	int m_interactRayId = -100; //interact
	int m_lastInteractRayId = -100;
	int m_objectInfoRayId = -100; // objectInfo

	PhysicsComponent* m_head = nullptr;
public:
	//Magic number
	static const int g_fullVisability = 1000;

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
	void setPosition(const DirectX::XMFLOAT4A& pos) override;

	void setPlayAnimation(bool playAnimation); 

	void Draw() override;
	void DrawHUDComponents();

	//Networking
	void SendOnUpdateMessage();
	void SendOnAbilityUsed();
	void SendAbilityUpdates();
	void SendOnAnimationUpdate(double dt);
	void SendOnWinState();
	void RegisterThisInstanceToNetwork();

	void SetCurrentVisability(const float & guard);
	void SetCurrentSoundPercentage(const float & percentage);
	void SetFirstPersonModel();
	void setHeadbobbingActive(bool active); 
	void LockPlayerInput();
	bool IsInputLocked();
	void UnlockPlayerInput();

	void setExitPause(bool exitPause);

	const bool& getHeadbobbingActive() const;
	const bool& getExitPause() const; 
	const float & getVisability() const;
	const int & getFullVisability() const;
	const bool & getWinState() const { return hasWon; }
	const bool & getPlayerLocked() const; 
	Animation::AnimationPlayer* GetFirstPersonAnimationPlayer();

	const AudioEngine::Listener & getFMODListener() const; 
	AudioEngine::SoundDesc& getSmokeBombDesc(); 

	
	//This is a way of checking if we can use the ability with out current mana
	void SetAbilitySet(int set);
	void SetModelAndTextures(int set);
	void setEnemyPositions(std::vector<Enemy *> & enemys);

	const Ability getCurrentAbility()const;
	TeleportAbility * getTeleportAbility();
	unsigned int getNrOfRocks();
	bool GetMapPicked();
	const int getInteractRayId();
	const bool sameInteractRayId(int id);

	void InstaWin();
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
	
	void _updateFirstPerson(float deltaTime);
	void _cameraPlacement(double deltaTime);
	void _updateFMODListener(double deltaTime, const DirectX::XMFLOAT4A & xmLastPos);
	void _activateCrouch(const unsigned int inputType);
	void _deActivateCrouch();
	void _startSprint(const unsigned int inputType);
	void _startWalk();
	void _hasWon();
	b3Vec3 _slerp(b3Vec3 start, b3Vec3 end, float percent);
	void _loadHUD();
	void _unloadHUD();
	void _initSoundHUD();
	//Cheats, like changing ability set
	void _cheats();
	void _drawCollisionBox(b3Shape* shape);
};
