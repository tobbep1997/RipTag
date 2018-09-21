#pragma once
#include "State.h"

class PlayState : public State
{
private:


public:
	PlayState(RenderingManager * rm);
	~PlayState();

	void Update() override;

	void Draw() override;
};
