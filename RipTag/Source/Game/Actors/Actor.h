#pragma once
#include "../Engine/Source/3D Engine/Model/Model.h"
class Actor : public Model
{
protected:
public:      
	Actor();
	virtual  ~Actor();

	virtual void BeginPlay() = 0;
	virtual void Update(double deltaTime) = 0;
};
