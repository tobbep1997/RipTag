#pragma once
#include "../Level/Room.h"
#include <Vector>
#include "../Actors/BaseActor.h"
#include <filesystem>
#include <iostream>
#include <future>


class LevelHandler
{
private:
	const std::string m_roomString = "../Assets/ROOMSPREFAB";
	std::vector<std::string> m_prefabRoomFiles;

	short unsigned int m_activeRoom;
	std::vector<Room*> m_rooms;

	bool pressed = false; 

	b3World * m_worldPtr;

private:
	std::vector<int> m_unloadingQueue;
	std::mutex m_unloadMutex;
	std::vector<int> m_loadingQueue;
	std::mutex m_loadMutex;

	std::future<void> future;
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
	void _RoomLoadingThreading();
};
