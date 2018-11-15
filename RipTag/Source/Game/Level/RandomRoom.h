#pragma once

enum RoomType
{
	HORIZONTAL_ROOM,
	VERTICAL_ROOM,
	NORMAL_ROOM
};

class RandomRoom
{
public:
	bool north, east, south, west;
	int leadsToRoom[4];
	RoomType type;

public:
	RandomRoom(bool _north = false, bool _east = false, bool _south = false, bool _west = false)
	{
		north = _north; east = _east; south = _south; west = _west;
		for (int i = 0; i < 4; i++) { leadsToRoom[i] = -1; }
		type = NORMAL_ROOM;
	}
	~RandomRoom() {}

};