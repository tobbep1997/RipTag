#pragma once
//#include "../Engine/EngineSource/3D Engine/Components/Camera.h"
#include "../Actor.h"
//#include "../Engine/EngineSource/3D Engine/Model/Model.h"
#include "EngineSource/3D Engine/Components/Base/CameraHolder.h"
#include "EngineSource/3D Engine/3DRendering/Rendering/VisabilityPass/Component/VisibilityComponent.h"

class Enemy : public Actor, public CameraHolder
{
private:
	VisibilityComponent m_vc;
	bool m_allowVisability = false;

public:
	Enemy();
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

	//Depending on the culling, this can cancel the queue
	void QueueForVisibility();
};

