#pragma once
#include "../Level/Room.h"
#include <Vector>

class LevelHandler
{
private:
	std::vector<Room*> m_rooms;

public:
	LevelHandler();
	~LevelHandler();

	void Update();

	void Draw();

private:

};
