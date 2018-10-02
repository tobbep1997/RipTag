#pragma once
#include "NetworkIDObject.h"
#include <Multiplayer.h>
#include "../Engine/Source/3D Engine/Model/Model.h"

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#define PROTOTYPE_METATABLE "PrototypeMT"
#define PROTOTYPE_NEW "Prototype"
#define PROTOTYPE_NEW_REMOTE "PrototypeRemote"
#define PROTOTYPE_SET_POS "SetPosition"
#define PROTOTYPE_GET_NID "GetNID"

class CubePrototype : public RakNet::NetworkIDObject
{
public:
	CubePrototype(float x, float y, float z);
	CubePrototype(RakNet::NetworkID nid, float x, float y, float z);
	~CubePrototype();

	void setPosition(DirectX::XMFLOAT4A pos);
	void Draw();
	
private:
	Model m;
	StaticMesh * s;
};


static int New_Prototype(lua_State * L)
{
	float x = lua_tonumber(L, -3);
	float y = lua_tonumber(L, -2);
	float z = lua_tonumber(L, -1);

	CubePrototype * ptr = new CubePrototype(x, y, z);
	lua_pushlightuserdata(L, (void*)ptr);
	luaL_setmetatable(L, PROTOTYPE_METATABLE);

	return 1;
}

static int New_Prototype_Network(lua_State * L)
{
	Network::ENTITY_CREATE_MESSAGE * data = (Network::ENTITY_CREATE_MESSAGE *)lua_touserdata(L, lua_gettop(L));
	CubePrototype * ptr = 0;
	if (data)
	{
		ptr = new CubePrototype(data->nId, data->x, data->y, data->z);
		lua_pushlightuserdata(L, (void*)ptr);
		luaL_setmetatable(L, PROTOTYPE_METATABLE);
		return 1;
	}
	return 0;
}

static int Set_Position_Prototype(lua_State * L)
{
	//vi borde anv�nda NID f�r detta s� vi kommer �t pekaren via NetworkIDManager
	//men f�r nu anv�nd pekaren, idk
	CubePrototype * ptr = (CubePrototype*)lua_touserdata(L, -4);

	float x = (float)lua_tonumber(L, -3);
	float y = (float)lua_tonumber(L, -2);
	float z = (float)lua_tonumber(L, -1);

	lua_pop(L, 4);

	if (ptr)
	{
		ptr->setPosition(DirectX::XMFLOAT4A(x, y, z, 1.0f));
		return 0;
	}
	return 0;
}

static int Get_NID_Prototype(lua_State *L)
{
	CubePrototype * ptr = (CubePrototype*)lua_touserdata(L, -1);
	lua_pop(L, 1);

	if (ptr)
	{
		lua_pushnumber(L, (double)ptr->GetNetworkID());
		return 1;
	}
	return 0;
}

static void LUA_Register_CubePrototype(lua_State *L)
{
	lua_register(L, PROTOTYPE_NEW, New_Prototype_Network);
	luaL_newmetatable(L, PROTOTYPE_METATABLE);
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, Set_Position_Prototype); lua_setfield(L, -2, PROTOTYPE_SET_POS);
	lua_pushcfunction(L, Get_NID_Prototype); lua_setfield(L, -2, PROTOTYPE_GET_NID);
	lua_pop(L, 1);
}
