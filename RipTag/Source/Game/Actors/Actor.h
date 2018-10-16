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
