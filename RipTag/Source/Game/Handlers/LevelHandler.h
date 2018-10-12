#pragma once
#include "../Level/Room.h"
#include <Vector>

class LevelHandler
{
private:
	std::vector<Room*> m_rooms;

	b3World * worldPtr;

public:
	LevelHandler();
	~LevelHandler();

	void Init(b3World & worldPtr);

	void Update();

	void Draw();

	
	void UnloadRoom(const int roomNumber);
private:
	void _LoadRoom(const int roomIndex);
};
