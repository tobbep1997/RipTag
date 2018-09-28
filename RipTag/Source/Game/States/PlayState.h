#pragma once
#include "State.h"
#include "../Handlers/CameraHandler.h"
#include "../Handlers/LevelHandler.h"
#include "../Handlers/ObjectHandler.h"
#include "../Actors/Player.h"
#include "../Actors/Enemy/Enemy.h"

class PlayState : public State
{
private:
	LevelHandler m_levelHandler;
	ObjectHandler m_objectHandler;

	Player * player;

	Enemy * enemy;
	
public:
	PlayState(RenderingManager * rm);
	~PlayState();

	void Update(double deltaTime) override;

	void Draw() override;

private:
};
