#pragma once
#include "../Level/Room.h"
#include <Vector>
#include "../Actors/BaseActor.h"

class LevelHandler
{
private:
	std::vector<Room*> m_rooms;

	b3World * m_worldPtr;
	BaseActor * testCube;

	StaticAsset testtt;
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
};
