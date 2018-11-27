#pragma once

#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include "RandomRoom.h"

struct RoomLocations
{
	RoomType type;
	int startIndex;
	int endIndex;

	RoomLocations() {};
	RoomLocations(RoomType _type, int start, int end) { type = _type; startIndex = start; endIndex = end; };
};

class RandomRoomGrid
{
private:
	//int currentGridLocation = 0;
	int m_gridSize;
	int m_depth;
	int m_width;
	int * m_roomGrid;
	std::vector<RoomLocations> m_oddRooms;

public:
	RandomRoom * m_rooms;

public:
	RandomRoomGrid(int width = 5, int depth = 5);
	~RandomRoomGrid();

	void GenerateRoomLayout();
	DirectX::XMINT2 GetSize() const;

	void Print();
	void DrawConnections();
	void drawEachRoom();
private:
	void _insertRooms(int count);
	void _connectRooms();
	// Send in valid directions in the order North -> East -> South -> West
	void _generateDoors(bool * validDirections, int doorCount, int roomIndex, int * directionIndex);
	void _forcePath(bool * visited);
	void _checkConnections();
	void _followConnection(bool * visited, int roomIndex);
	void _createRoomConnection(int start, int end);
};