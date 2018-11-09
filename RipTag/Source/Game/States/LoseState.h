#pragma once
#include "State.h"

class RenderingManager;
class LoseState :
	public State
{
public:
	LoseState(RenderingManager * rm);
	~LoseState();

	void Update(double deltaTime);

	void Draw() override;
};

