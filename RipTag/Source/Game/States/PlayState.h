#pragma once
#include "State.h"
#include "../Handlers/CameraHandler.h"

class PlayState : public State
{
private:
	Camera * temp;
public:
	PlayState(RenderingManager * rm);
	~PlayState();

	void Update() override;

	void Draw() override;
};
