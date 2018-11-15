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
	RandomRoom(bool _north = false, bool _east = false, bool _south = false, bool _west = false);
	~RandomRoom();

};