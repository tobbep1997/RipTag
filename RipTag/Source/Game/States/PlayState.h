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
	/*b3Body* m_body;
	b3Polyhedron * poly;
	b3Hull * bodyBox;
	b3BodyDef * bodyDef;
	b3ShapeDef* bodyBoxDef;
	b3Shape *	m_shape;*/

	b3Body*		m_floor;
	b3Polyhedron * poly2;
	b3Hull * bodyBox2;
	b3BodyDef * bodyDef2;
	b3ShapeDef* bodyBoxDef2;
	b3Shape * m_shape2;


	//Model * temp;

	//-----------------------------------------------------------------------------
	BaseActor * actor;

	float x = -1.5;
	float y = 2.1; 
	float z = -2.1;
	float ax = -1.5;
	float ay = 2.1;
	float az = -2.1;
	float adirX, adirY, adirZ;

	float intensity = 2;
	BaseActor * wall1;

	PointLight light1;
	PointLight light2;

	Guard gTemp;

	Model * model;

	Model * jumper;
	Animation::Skeleton* skeleton = nullptr;
	Animation::AnimationClip* animation = nullptr;
public:
	PlayState(RenderingManager * rm);
	~PlayState();

	void Update(double deltaTime) override;

	void Draw() override;

private:
};
