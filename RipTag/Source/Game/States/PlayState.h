#pragma once
#include "State.h"
#include "../Handlers/CameraHandler.h"
#include "../Handlers/LevelHandler.h"
#include "../Handlers/ObjectHandler.h"
#include "../Actors/Player.h"
#include "../Actors/Enemy/Enemy.h"
#include "../../../../Engine/Source/3D Engine/RenderingManager.h"

#include "../../Physics/Bounce.h"
#include "../Actors/BaseActor.h"
#include <future>

class PlayState : public State
{
private:
	LevelHandler m_levelHandler;
	ObjectHandler m_objectHandler;

	Player * player;

	Enemy * enemy;
	b3World m_world;


	//-----------------------------------------------------------------------------
	//PLEASE REMOBE THIS //TODO::PLEASE

	b3Body*		m_floor;
	b3Polyhedron * poly2;
	b3Hull * bodyBox2;
	b3BodyDef * bodyDef2;
	b3ShapeDef* bodyBoxDef2;
	b3Shape * m_shape2;

	//-----------------------------------------------------------------------------
	BaseActor * actor;

	float x = -1.5;
	float y = 2.1; 
	float z = -2.1;

	float intensity = 2;
	BaseActor * wall1;

	PointLight light1;
	PointLight light2;

	Guard gTemp;

	Model * model;
	//std::future<void> future;
	//std::thread test;
public:
	PlayState(RenderingManager * rm);
	~PlayState();

	void Update(double deltaTime) override;

	void Draw() override;

private:
	void thread(std::string s);
};
