#pragma once
#include "../Actor.h"
#include "EngineSource/3D Engine/Components/Base/CameraHolder.h"
#include "EngineSource/3D Engine/3DRendering/Rendering/VisabilityPass/Component/VisibilityComponent.h"
#include "../../../Physics/Wrapper/PhysicsComponent.h"
#include "../../Pathfinding/Grid.h"
#include "2D Engine/Quad/Components/HUDComponent.h"

class Enemy : public Actor, public CameraHolder, public PhysicsComponent
{
private:
	const float MOVE_SPEED = 10.0f;
	const float SPRINT_MULT = 2.0f;
	const float JUMP_POWER = 400.0f;

private:
	struct KeyPressedEnemy
	{
		bool jump = false;
		bool crouching = false;
		bool possess = false;
		bool unlockMouse = false;
		bool interact = false;
	};

	VisibilityComponent m_vc;
	bool m_allowVisability = false;

	bool m_inputLocked = true;

	bool m_disabled = false;

	float m_moveSpeed = 10;
	float m_camSensitivity = 5;
	float m_standHeight;
	float m_offPutY = 0.4f;
	float m_walk = 0;
	bool forward = true;
	float distance = 0.1f;

	//Possess
	Actor* m_possessor;
	float m_possessReturnDelay;
	float m_maxPossessDuration;
	
	//Key Input
	bool m_currClickCrouch = false;
	bool m_prevClickCrouch = false;
	bool m_currClickSprint = false;
	bool m_prevClickSprint = false;
	bool m_isSprinting = false;

	int m_toggleCrouch = 0;
	int m_toggleSprint = 0;
	KeyPressedEnemy m_kp;



	std::vector<Node*> m_path;

	float m_guardSpeed = 1.5;

	float m_visCounter;
	float m_visabilityTimer = 0.6f;

	bool m_found = false;

	
public:
	Enemy();
	Enemy(float startPosX, float startPosY, float startPosZ);
	Enemy(b3World* world, float startPosX, float startPosY, float startPosZ);
	~Enemy();

	//TEMP
	void setDir(const float & x, const float & y, const float & z);
	Camera * getCamera();
	const int* getPlayerVisibility() const;
	bool unlockMouse = false;

	// Inherited via Actor

	void CullingForVisability(const Transform & player);

	virtual void setPosition(const DirectX::XMFLOAT4A & pos) override;
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

	void SetPathVector(std::vector<Node*>  path);
	std::vector<Node*> GetPathVector();

	bool getIfLost();
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
	bool _MoveTo(Node * nextNode, double deltaTime);

	void _CheckPlayer(double deltaTime);
	void _activateCrouch();
	void _deActivateCrouch();
};

