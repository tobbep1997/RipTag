#pragma once

#include "../Actor.h"
#include "EngineSource/3D Engine/Components/Base/CameraHolder.h"
#include "../../../Physics/Wrapper/PhysicsComponent.h"

struct Node;

class VisibilityComponent;
class Grid;

enum EnemyState
{
	Investigating_Sight,
	Investigating_Sound,
	Patrolling
};

class Enemy : public Actor, public CameraHolder, public PhysicsComponent
{
public:
	struct SoundLocation
	{
		float percentage;
		DirectX::XMFLOAT3 soundPos;
	};

public:
	enum KnockOutType
	{
		Stoned,
		Possessed
	};

private:
	const float MOVE_SPEED = 5.0f;
	const float SPRINT_MULT = 2.0f;
	const float JUMP_POWER = 400.0f;


private:
	struct AudioVars
	{
		double timer = 0.0;
		float lastCurve = 0.0f;
		int lastIndex = 0;
		bool hasPlayed = false;
	};
	struct KeyPressedEnemy
	{
		bool jump = false;
		bool crouching = false;
		bool possess = false;
		bool unlockMouse = false;
		bool interact = false;
	};
	AudioVars m_av;
	KnockOutType m_knockOutType; 

	VisibilityComponent * m_vc;
	bool m_allowVisability = false;

	bool m_inputLocked = true;
	bool m_disabled = false;
	bool m_released = false; 

	bool m_justReleased = false; 

	float m_moveSpeed = 2;
	float m_cameraOffset;
	float m_camSensitivity = 5;
	float m_offPutY = 0.4f;
	float m_walk = 0;
	bool forward = true;
	float distance = 0.1f;
	float m_guardSpeed = 1.5;

	//Possess
	Actor* m_possessor;
	float m_possessReturnDelay;
	
	//Key Input
	bool m_currClickCrouch = false;
	bool m_prevClickCrouch = false;
	bool m_currClickSprint = false;
	bool m_prevClickSprint = false;
	bool m_isSprinting = false;

	int m_toggleCrouch = 0;
	int m_toggleSprint = 0;
	KeyPressedEnemy m_kp;

	float m_standHeight;
	float m_crouchHeight;
	float m_crouchAnimStartPos;

	bool m_alert = false;
	int m_currentPathNode = 0;
	int m_currentAlertPathNode = 0;
	std::vector<Node*> m_path;
	std::vector<Node*> m_alertPath;
	bool m_isReversed = false;

	EnemyState m_state = Patrolling;
	SoundLocation m_sl;

	float m_visCounter;
	float m_visabilityTimer = 1.6f;

	bool m_found = false;

	float m_knockOutTimer = 0;
	float m_possesionRecoverTimer = 0; 
	float m_possessionRecoverMax = 5; 
	float m_knockOutMaxTime = 2;

	float enemyX = 0;
	float enemyY = 0;

	std::vector<DirectX::BoundingSphere*> m_teleportBoundingSphere;
	DirectX::BoundingFrustum * m_boundingFrustum;

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

	Player * m_PlayerPtr;
public:
	Enemy();
	Enemy(float startPosX, float startPosY, float startPosZ);
	Enemy(b3World* world, float startPosX, float startPosY, float startPosZ);
	~Enemy();

	//TEMP
	void setDir(const float & x, const float & y, const float & z);
	Camera * getCamera();
	const int * getPlayerVisibility() const;
	bool unlockMouse = false;

	// Inherited via Actor

	void CullingForVisability(const Transform & player);

	DirectX::XMFLOAT2 GetDirectionToPlayer(const DirectX::XMFLOAT4A & player, Camera & playerCma);

	virtual void setPosition(const float & x, const float & y, const float & z, const float & w = 1.0f) override;
	virtual void BeginPlay() override;
	virtual void Update(double deltaTime) override;
	virtual void PhysicsUpdate(double deltaTime);

	//Depending on the culling, this can cancel the queue
	void QueueForVisibility();

	void LockEnemyInput();
	void UnlockEnemyInput();

	void DisableEnemy();
	void EnableEnemy();
	bool GetDisabledState();

	void _IsInSight();

	Enemy * validate();

	void setPossessor(Actor* possessor, float maxDuration, float delay);
	void removePossessor();

	//0 is Stoned, 1 is exit-possess cooldown
	void setKnockOutType(KnockOutType knockOutType);

	void SetPathVector(std::vector<Node*>  path);
	Node * GetCurrentPathNode() const;

	void SetAlertVector(std::vector<Node*> alertPath);
	void setReleased(bool released); 
	size_t GetAlertPathSize() const;
	Node * GetAlertDestination() const;

	EnemyState getEnemyState() const;
	void setEnemeyState(EnemyState state);

	void setSoundLocation(const SoundLocation & sl);
	const SoundLocation & getSoundLocation() const;

	bool getIfLost();
	const KnockOutType getKnockOutType() const; 

	float getTotalVisablilty() const;
	float getMaxVisability() const;
	float getVisCounter() const;
	void addTeleportAbility(const TeleportAbility & teleportAbility);

	void DrawGuardPath();
	void EnableGuardPathPrint();

	void SetLenghtToPlayer(const DirectX::XMFLOAT4A & playerPos);

	void SetPlayerPointer(Player * player);
private:

	void _handleInput(double deltaTime);
	//Movement
	void _handleMovement(double deltaTime);
	void _handleRotation(double deltaTime);

	void _TempGuardPath(bool x, double deltaTime);
	void _possessed(double deltaTime);
	void _onCrouch();
	void _onJump();
	void _onSprint();
	void _cameraPlacement(double deltaTime);
	bool _MoveTo(Node * nextNode, double deltaTime);
	bool _MoveToAlert(Node * nextNode, double deltaTime);
	void _MoveBackToPatrolRoute(Node * nextNode, double deltaTime);
	void _RotateGuard(float x, float y, float angle, float deltaTime);

	void _CheckPlayer(double deltaTime);
	void _activateCrouch();
	void _deActivateCrouch();

	float _getPathNodeRotation(DirectX::XMFLOAT2 first, DirectX::XMFLOAT2 last);

	void _playFootsteps(double deltaTime);
};

