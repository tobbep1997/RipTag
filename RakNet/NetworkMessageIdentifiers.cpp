#include "NetworkMessageIdentifiers.h"

int Network::Packets::New_Game_Message(lua_State * L)
{
	unsigned char id = (unsigned char)lua_tonumber(L, -1);
	lua_pop(L, -1);

	GAME_MESSAGE * ptr = new GAME_MESSAGE(id);

	lua_pushlightuserdata(L, (void*)ptr);
	lua_pushnumber(L, sizeof(GAME_MESSAGE));

	return 2;
}

int Network::Packets::Destroy_Game_Message(lua_State * L)
{
	GAME_MESSAGE * ptr = (GAME_MESSAGE*)lua_touserdata(L, -1);
	delete ptr; ptr = 0;
	return 0;
}

void Network::Packets::REGISTER_TO_LUA()
{
	static sol::usertype<Packets> object(
		"new", sol::no_constructor,
		"GameMessage", &New_Game_Message,
		"DestroyGameMessage", &Destroy_Game_Message
		);

	LUA::LuaTalker * m_talker = LUA::LuaTalker::GetInstance();
	m_talker->defineObjectToLua("Packets", object);
}

void Network::PacketIdentifiers::REGISTER_TO_LUA()
{
	LUA::LuaTalker * m_talker = LUA::LuaTalker::GetInstance();
	m_talker->getSolState().new_enum(LUA_TABLE_MESSAGE_IDENTIFIERS,
		ENUM_TO_STR(ID_GAME_START), ID_GAME_START);
}
