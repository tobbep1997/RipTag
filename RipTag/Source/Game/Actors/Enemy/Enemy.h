#pragma once
//#include "../Engine/EngineSource/3D Engine/Components/Camera.h"
#include "../Actor.h"
//#include "../Engine/EngineSource/3D Engine/Model/Model.h"
#include "EngineSource/3D Engine/Components/Base/CameraHolder.h"
#include "EngineSource/3D Engine/3DRendering/Rendering/VisabilityPass/Component/VisibilityComponent.h"
#include "../../../Physics/Wrapper/PhysicsComponent.h"

class Enemy : public Actor, public CameraHolder, public PhysicsComponent
{
private:
	VisibilityComponent m_vc;
	bool m_allowVisability = false;

	bool m_inputLocked = true;

	bool m_disabled = false;

	float m_movementSpeed = 10;

	float m_walk = 0;
	bool forward = true;
	float distance = 0.1f;
	float m_speed = 0.5f;
	Actor* m_possessor;
public:
	Enemy();
	Enemy(float startPosX, float startPosY, float startPosZ);
	Enemy(b3World* world, float startPosX, float startPosY, float startPosZ);
	~Enemy();

	//TEMP
	void setDir(const float & x, const float & y, const float & z);
	Camera * getCamera();
	const int* getPlayerVisibility() const;

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

	void setPossessor(Actor* possessor);
private:

	void _handleInput(double deltaTime);
	//Movement
	void _handleMovement(double deltaTime);
	void _handleRotation(double deltaTime);

	void _TempGuardPath(bool x, double deltaTime);
	void _possessed();
};

