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
#include "../Actors/Enemy/Enemy.h"


//lua 
#include <LuaTalker.h>

#define LUA_PLAYSTATE "PlayState"


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

	float x = -1.5f;
	float y = 2.1f; 
	float z = -2.1f;

	float xD = 0;
	float yD = 0;
	float zD = 0;

	float intensity = 2;
	BaseActor * wall1;

	BaseActor * testCube;

	PointLight light1;
	PointLight light2;

	Enemy gTemp;

	Model * model;
	//std::future<void> future;
	//std::thread test;
	b3TimeStep m_step;
	bool m_firstRun = true;
public:
	PlayState(RenderingManager * rm);
	~PlayState();

	void Update(double deltaTime) override;

	void Draw() override;

private:
	void thread(std::string s);
};

static int New_PlayState(lua_State * L)
{
	RenderingManager * ptr = (RenderingManager*)lua_touserdata(L, -1);
	lua_pop(L, 1);

	PlayState * state = 0;
	if (ptr)
	{
		state = new PlayState(ptr);
		lua_pushlightuserdata(L, (void*)state);
	}
}

static void LUA_Register_PlayState(lua_State * L)
{
	lua_register(L, LUA_PLAYSTATE, New_PlayState);
	luaL_newmetatable(L, LUA_STATE_METATABLE);

}

