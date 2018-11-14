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
	~RoomLocations() {};
};

class RandomRoomGrid
{
private:
	static const int GRID_SIZE = 25;
	int currentGridLocation = 0;
	int depth = 5;
	int width = 5;

	void _insertRooms(int count);

public:
	int roomGrid[GRID_SIZE];
	std::vector<RoomLocations> oddRooms;

	RandomRoom rooms[GRID_SIZE];

public:
	RandomRoomGrid() { for (int i = 0; i < GRID_SIZE; i++) { roomGrid[i] = 2; } }
	~RandomRoomGrid() {}

	void GenerateRoomLayout();

	void Print();
	void DrawConnections();

private:
	void _connectRooms();
	// Send in valid directions in the order North -> East -> South -> West
	void _generateDoors(bool validDirections[4], int doorCount, int roomIndex, int directionIndex[4]);
	void _forcePath(bool visited[GRID_SIZE]);
	void _checkConnections();
	void _followConnection(bool visited[GRID_SIZE], int roomIndex);
	void _createRoomConnection(int start, int end);
};