#pragma once
//#include "Source/3D Engine/RenderingManager.h"

#include "../../../Engine/Source/3D Engine/RenderingManager.h"

class State
{
private:

protected:
	RenderingManager * p_renderingManager;

public:
	State(RenderingManager * rm = nullptr);
	virtual ~State();

	virtual void Update() = 0;

	virtual void Draw() = 0;
};
