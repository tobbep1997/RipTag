#pragma once

#include "../Handlers/PlayerManager.h"
#include <future>
#include "State.h"

#include "../../Physics/Bounce.h"
#include "../Handlers/CameraHandler.h"
#include "../Handlers/LevelHandler.h"
#include "../Actors/BaseActor.h"
#include "../Actors/Enemy/Enemy.h"
#include "2D Engine/Quad/Quad.h"
#include "../../../Engine/EngineSource/3D Engine/Extern.h"


//lua 
#include <LuaTalker.h>
#include "../../Physics/Wrapper/ContactListener.h"

#include "../../Gameplay/Triggers/TriggerHandler.h"
#define LUA_PLAYSTATE "PlayState"

#include "../../Gameplay/Objects/PressurePlate.h"
#include "../../Gameplay/Objects/Lever.h"
#include "../../Gameplay/Objects/Door.h"

class PlayState : public State
{
private:
	LevelHandler m_levelHandler;
	ContactListener * m_contactListener;
	RayCastListener * m_rayListener;

	PlayerManager * m_playerManager;

	b3World m_world;

	TriggerHandler *	triggerHandler;
	PressurePlate *		pressureplate;
	Lever *				lever;
	Door *				door;
	float intensity = 2;
	Drawable * model;
	b3TimeStep m_step;
	bool m_firstRun = true;
	bool unlockMouse = true;
	FMOD::Channel * TEEEMPCHANNEL;


public:

	PlayState(RenderingManager * rm);
	~PlayState();

	void Update(double deltaTime) override;

	void Draw() override;

private:
	void thread(std::string s);
	void TemporaryLobby();
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

