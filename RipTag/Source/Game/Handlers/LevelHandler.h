#pragma once
#include <Vector>
#include <filesystem>
#include <iostream>
#include <future>

class Room;
class Player;

class LevelHandler
{
private:
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

	void Update(float deltaTime);

	void Draw();

	void setPlayer(Player * playerPtr);

	const std::vector<Enemy*>* getEnemies() const;
private:

	void _LoadPreFabs();
	void _GenerateLevelStruct(const int seed, const int amountOfRooms = 5);

	void _RoomLoadingManager(short int room = -1);
	void _RoomLoadingThreading();
};
