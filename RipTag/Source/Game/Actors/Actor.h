#pragma once
#include "EngineSource/3D Engine/Components/Base/Drawable.h"

class Actor : public Drawable
{
protected:
public:      
	Actor();
	virtual  ~Actor();

	virtual void BeginPlay() = 0;
	virtual void Update(double deltaTime) = 0;
};

//We need to create a state machine for animations etc. 
enum PlayerState
{
	Idle,
	Walking,
	Sprinting,
	Crouching,
	Jumping,
	Falling,
	InBlink,
	InTeleport,
	CheckingVisibility
};