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

	short unsigned int m_activeRoom;
	std::vector<Room*> m_rooms;

	

	b3World * m_worldPtr;

private:
	std::vector<int> m_unloadingQueue;
	std::vector<int> m_loadingQueue;

public:
	LevelHandler();
	~LevelHandler();

	void Init(b3World & worldPtr);
	void Release();

	void Update(float deltaTime);

	void Draw();

private:

	void _LoadPreFabs();
	void _GenerateLevelStruct(const int seed, const int amountOfRooms = 5);

	void _RoomLoadingManager(short int room = -1);
};
