#pragma once

#include <future>
#include "State.h"
#include "../../Physics/Bounce.h"
#include "../Handlers/CameraHandler.h"
#include "../Handlers/LevelHandler.h"
#include "../Handlers/ObjectHandler.h"
#include "../Actors/Player.h"
#include "../Actors/BaseActor.h"
#include "../Actors/Enemy/Enemy.h"

class PlayState : public State
{
private:
	LevelHandler m_levelHandler;
	ObjectHandler m_objectHandler;

	Player * player;

	Enemy * enemy;
	b3World m_world;

	
	BaseActor * actor;

	BaseActor * CollisionBoxes;

	float x = -1.5f;
	float y = 2.1f; 
	float z = -2.1f;

	float xD = 0;
	float yD = 0;
	float zD = 0;

	float intensity = 2;
	
	
	PointLight light1;
	PointLight light2;

	Enemy gTemp;

	Drawable * model;
	//std::future<void> future;
	//std::thread test;
	b3TimeStep m_step;
	bool m_firstRun = true;

	

	bool unlockMouse = true;
public:
	PlayState(RenderingManager * rm);
	~PlayState();

	void Update(double deltaTime) override;

	void Draw() override;

private:
	void thread(std::string s);
};
