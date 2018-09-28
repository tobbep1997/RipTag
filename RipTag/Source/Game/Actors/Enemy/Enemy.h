#pragma once
#include "../Engine/Source/3D Engine/Camera.h"
#include "../Actor.h"
#include "../Engine/Source/3D Engine/Model/Model.h"
#include "Source/3D Engine/Components/CameraHolder.h"
class Enemy : public Actor, public CameraHolder
{
public:
	Enemy();
	~Enemy();

	// Inherited via Actor
	virtual void BeginPlay() override;
	virtual void Update(double deltaTime) override;
};

