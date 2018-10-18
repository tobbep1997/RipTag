#include "NetworkMessageIdentifiers.h"


std::tuple<void*, size_t> Network::Packets::New_Game_Message(unsigned char _id)
{
	GAME_MESSAGE * ptr = new GAME_MESSAGE(_id);
	return std::make_tuple((void*)ptr, sizeof(GAME_MESSAGE));
}

void Network::Packets::Destroy_Game_Message(void * in_ptr)
{
	GAME_MESSAGE * ptr = (GAME_MESSAGE*)in_ptr;
	delete ptr; ptr = 0;
}

void Network::Packets::REGISTER_TO_LUA()
{
	static bool isRegged = false;

	if (!isRegged)
	{
		LUA::LuaTalker * talker = LUA::LuaTalker::GetInstance();
		sol::state_view * solStateView = talker->getSolState();

		solStateView->new_usertype<Packets>(LUA_PACKETS_METATABLE,
			"new", sol::no_constructor,
			LUA_GAME_MESSAGE_PACKET, &Packets::New_Game_Message,
			LUA_DESTROY_GAME_MESSAGE, &Packets::Destroy_Game_Message
			);

		
		talker->getSolState()->new_enum(LUA_TABLE_MESSAGE_IDENTIFIERS,
			ENUM_TO_STR(ID_GAME_START), ID_GAME_START);


		isRegged = true;
	}


}


