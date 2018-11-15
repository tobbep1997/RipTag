#include "RipTagPCH.h"

RandomRoom::RandomRoom(bool _north, bool _east, bool _south, bool _west)
{
	north = _north; east = _east; south = _south; west = _west;
	for (int i = 0; i < 4; i++) { leadsToRoom[i] = -1; }
	type = NORMAL_ROOM;
}

RandomRoom::~RandomRoom()
{
}
