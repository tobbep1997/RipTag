#pragma once
//#include "../Engine/EngineSource/3D Engine/Components/Camera.h"
#include "../Actor.h"
//#include "../Engine/EngineSource/3D Engine/Model/Model.h"
#include "EngineSource/3D Engine/Components/Base/CameraHolder.h"
#include "EngineSource/3D Engine/3DRendering/Rendering/VisabilityPass/Component/VisibilityComponent.h"
#include "../../../Physics/Wrapper/PhysicsComponent.h"
#include "../../Pathfinding/Grid.h"


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

	float m_movementSpeed = 10;
	float m_camSensitivity = 5;
	float m_standHeight;
	float m_walk = 0;
	bool forward = true;
	float distance = 0.1f;
	Actor* m_possessor;
	float m_possessReturnDelay;
	float m_maxPossessDuration;
	KeyPressedEnemy m_kp;

	std::vector<Node*> m_path;

	float m_guardSpeed = 1.5;
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
};

