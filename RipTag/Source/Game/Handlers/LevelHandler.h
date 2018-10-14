#pragma once
#include "../Level/Room.h"
#include <Vector>
#include "../Actors/BaseActor.h"
#include <filesystem>
#include <iostream>


class LevelHandler
{
private:
	const std::string m_roomString = "../Assets/ROOMSPREFAB";
	std::vector<std::string> m_prefabRoomFiles;


	std::vector<Room*> m_rooms;

	

	b3World * m_worldPtr;

public:
	LevelHandler();
	~LevelHandler();

	void Init(b3World & worldPtr);
	void Release();

	void Update(float deltaTime);

	void Draw();

	
	void UnloadRoom(const int roomNumber);
private:
	void _LoadRoom(const int roomIndex);

	void _LoadPreFabs();
	void _GenerateLevelStruct(const int seed, const int amountOfRooms = 5);
};
