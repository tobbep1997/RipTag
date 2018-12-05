#pragma once

#include "../Actor.h"
#include "EngineSource/3D Engine/Components/Base/CameraHolder.h"
#include "../../../Physics/Wrapper/PhysicsComponent.h"
struct Node;

class VisibilityComponent;
class AI;
class Torch;

class Enemy : public Actor, public CameraHolder, public PhysicsComponent, public AI
{
public:

public:
	enum KnockOutType
	{
		Stoned,
		Possessed
	};

private:
	friend class AI;
	const float MOVE_SPEED = 4.0f;
	const float SPRINT_MULT = 2.0f;
	const float JUMP_POWER = 400.0f;
	const float INTERACT_RANGE = 3.0f;
	const float TURN_SPEED = 2.0f;
	const float REVERSE_SPEED = 0.5f;

	AudioEngine::SoundDesc m_soundFootstep;


	float m_currentSpeed = 0.0f;
	float m_currentDirection = 0.0f;
	bool m_IsPossessed = false;
private:
	struct AudioVars
	{
		double timer = 0.0;
		float lastCurve = 0.0f;
		int lastIndex = 0;
		bool hasPlayed = false;
	};

	struct lerpVal
	{
		bool newNode = true;
		bool turnState = false;
		bool next = false;
		float timer = 0.0f;
		DirectX::XMFLOAT2 lastDir = { 0.0f,0.0f };
		DirectX::XMFLOAT2 middleTarget = { 0.0f,0.0f };
	};
	unsigned int uniqueID;
	int m_rayId = -100;

	lerpVal m_lv;
	AudioVars m_av;
	KnockOutType m_knockOutType; 

	VisibilityComponent * m_vc;
	bool m_allowVisability = false;

	bool m_inputLocked = true;
	bool m_disabled = false;
	bool m_released = false; 
	bool m_lockedByClient = false;

	bool m_justReleased = false; 

	float m_moveSpeed = 4.0f;
	float m_scrollMoveModifier = 1.0f;
	float m_cameraSpeed = 1.0f;
	float m_offPutY = 0.4f; 
	float m_currentMoveSpeed = 0.0f;
	float m_LastFrameXPos = 0.0f;
	float m_LastFrameZPos = 0.0f;
	float m_walk = 0;
	bool forward = true;
	float distance = 0.1f;
	float m_guardSpeed = 1.5;

	//Possess
	Player* m_possessor;
	float m_possessReturnDelay;
	
	//Key Input
	bool m_currClickCrouch = false;
	bool m_prevClickCrouch = false;
	bool m_currClickSprint = false;
	bool m_prevClickSprint = false;
	bool m_isSprinting = false;

	int m_toggleCrouch = 0;
	int m_toggleSprint = 0;
	KeyPressed m_kp;

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


	SoundLocation m_sl = { 0.0f, {-999.0f, -999.0f, -999.0f} };
	SoundLocation m_slRemote = { 0.0f, {-999.0f, -999.0f, -999.0f} };
	SoundLocation m_loudestSoundLocation = { 0.0f, {-999.0f, -999.0f, -999.0f} };

	DirectX::XMFLOAT4A m_clearestPlayerPos;
	float m_biggestVisCounter = 0.0f;

	float m_visCounter;
	float m_visabilityTimer = 1.6f;

	bool m_found = false;

	float m_knockOutTimer = 0;
	float m_possesionRecoverTimer = 0; 
	float m_possessionRecoverMax = 5; 
	float m_knockOutMaxTime = 5.0;

	float enemyX = 0;
	float enemyY = 0;

	std::vector<DirectX::BoundingSphere*> m_teleportBoundingSphere;
	DirectX::BoundingFrustum * m_boundingFrustum;

	bool m_isReversed = false;
	const int m_maxDrawOutNode = 10;
	std::vector<Drawable*> m_pathNodes;
	float m_sinWaver = 0;

	bool m_nodeFootPrintsEnabled = false;

	const float m_startYPos = 4.5f;

	/*	Okey, do the lenght to the player is what it sounds like. Length span is just how close the player sould be. before the if state ment activates
	 *	So This is in _CheckPlayer, this will activate a multiply for the visPress 
	 */
	float m_lenghtToPlayer = 1000000000;
	float m_lengthToPlayerSpan = 8;

	Player * m_PlayerPtr		= nullptr;
	RemotePlayer * m_RemotePtr	= nullptr;

	int m_guardUniqueIndex = -1;

	int m_interactRayId = -100;

	std::vector<Torch*> m_torches;

public:
	Enemy(b3World* world, unsigned int id, float startPosX, float startPosY, float startPosZ);
	~Enemy();

	//TEMP
	void setDir(const float & x, const float & y, const float & z);
	Camera * getCamera();
	const int * getPlayerVisibility() const;
	bool unlockMouse = false;

	// Inherited via Actor

	void CullingForVisability(const Transform & player);

	virtual void setPosition(const float & x, const float & y, const float & z, const float & w = 1.0f) override;
	virtual void BeginPlay() override;
	virtual void Update(double deltaTime) override;
	void ClientUpdate(double deltaTime);
	virtual void PhysicsUpdate(double deltaTime);

	void Draw() override;

	//Depending on the culling, this can cancel the queue
	void QueueForVisibility();

	void LockEnemyInput();
	void UnlockEnemyInput();

	void DisableEnemy();
	void EnableEnemy();
	bool GetDisabledState();

	void _IsInSight();

	Enemy * validate();

	void setPossessor(Player* possessor, float maxDuration, float delay);
	void removePossessor();
	//0 is Stoned, 1 is exit-possess cooldown
	void setKnockOutType(KnockOutType knockOutType);

	void setReleased(bool released);
	bool ClientLocked() { return m_lockedByClient; }

	void setSoundLocation(const SoundLocation & sl);
	void setSoundLocationRemote(const SoundLocation & sl) { m_slRemote = sl; };
	const SoundLocation & getSoundLocation() const;

	const SoundLocation & getLoudestSoundLocation() const;
	void setLoudestSoundLocation(const SoundLocation & sl);
	void setCalculatedVisibilityFor(int playerIndex, int value);


	const DirectX::XMFLOAT4A & getClearestPlayerLocation() const;
	void setClearestPlayerLocation(const DirectX::XMFLOAT4A & cpl);

	const float & getBiggestVisCounter() const;
	void setBiggestVisCounter(float bvc);

	bool getIfLost();
	const KnockOutType getKnockOutType() const; 

	void addTeleportAbility(const TeleportAbility & teleportAbility);

	void DrawGuardPath();
	void EnableGuardPathPrint();

	float GetLenghtToPlayer(const DirectX::XMFLOAT4A & playerPos);

	void SetPlayerPointer(Player * player);
	void SetRemotePointer(RemotePlayer * remote) { m_RemotePtr = remote; }


	void _CheckPlayer(double deltaTime);
	unsigned int getUniqueID() const { return this->uniqueID; }

	//Network
	void onNetworkUpdate(Network::ENEMYUPDATEPACKET * packet);
	void onNetworkPossessed(Network::ENTITYSTATEPACKET * packet);
	void onNetworkDisabled(Network::ENTITYSTATEPACKET * packet);

	void sendNetworkUpdate();

	float getTotalVisibility();
	float getMaxVisibility();

	int GetGuardUniqueIndex();
	void SetGuardUniqueIndex(const int & index);
	const int getInteractRayId();

	void SetTorchContainer(std::vector<Torch*>& v) { m_torches = v; }
private:

	void _handleInput(double deltaTime);
	//Movement
	void _onMovement(double deltaTime); 
	void _scrollMovementMod();
	void _onCrouch();
	void _onJump(); 
	void _onSprint();
	void _onInteract();
	void _onRotate(double deltaTime); 

	void _onReleasePossessed(double deltaTime);
	void _cameraPlacement(double deltaTime); 
	void _RotateGuard(float x, float y, float angle, float deltaTime);
	void _activateCrouch();
	void _deActivateCrouch();

	void _playFootsteps(double deltaTime);
	b3Vec3 _slerp(b3Vec3 start, b3Vec3 end, float percent);

	void _detectTeleportSphere();
};