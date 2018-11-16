#pragma once
#include <filesystem>
#include <iostream>
#include <future>
#include "../Level/RoomGenerator.h"
class Room;
class Player;
class RoomGenerator;
class LevelHandler
{
private:
	RoomGenerator m_roomGenerator;

	const std::string m_roomString;
	std::vector<std::string> m_prefabRoomFiles;

	short unsigned int m_activeRoom;
	std::vector<Room*> m_rooms; //Released

	bool pressed = false; 

	Player * m_playerPtr; //Released in playstate
	b3World * m_worldPtr; //static in playsatet

private:
	std::vector<int> m_unloadingQueue;
	std::mutex m_unloadMutex;
	std::vector<int> m_loadingQueue;
	std::mutex m_loadMutex;

	std::future<void> future;
public:
	LevelHandler();
	~LevelHandler();

	void Init(b3World & worldPtr, Player * playerPtr);
	void Release();

	void Update(float deltaTime, Camera * camera);

	void Draw();

	void setPlayer(Player * playerPtr);

	const std::vector<Enemy*>* getEnemies() const;

	TriggerHandler * getTriggerHandler();

	std::tuple<DirectX::XMFLOAT4, DirectX::XMFLOAT4> getStartingPositions();
private:

	void _LoadPreFabs();
	void _GenerateLevelStruct(const int seed, const int amountOfRooms = 5);

	void _RoomLoadingManager(short int room = -1);
	void _RoomLoadingThreading();
};
