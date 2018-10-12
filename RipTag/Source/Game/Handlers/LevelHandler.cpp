#include "LevelHandler.h"

LevelHandler::LevelHandler()
{
}

LevelHandler::~LevelHandler()
{
	for (auto room : m_rooms)
	{
		delete room;
	}

	
}

void LevelHandler::Init(b3World& worldPtr)
{
	m_worldPtr = &worldPtr;

	testCube = new BaseActor();
	testCube->Init(*m_worldPtr, e_dynamicBody, 1.0f, 1.0f, 1.0f);
	testCube->setPosition(5, 5.0f, 0);
	testCube->setModel(Manager::g_meshManager.getStaticMesh("KUB"));
	testCube->setTexture(Manager::g_textureManager.getTexture("SPHERE"));


	//testtt.Init(m_worldPtr, );
}

void LevelHandler::Release()
{
	testCube->Release(*m_worldPtr);
	delete testCube;
}

void LevelHandler::Update(float deltaTime)
{
	for (unsigned int i = 0; i < m_rooms.size(); ++i)
	{
		m_rooms.at(i)->Update();
	}

	testCube->Update(deltaTime);
}

void LevelHandler::Draw()
{
	for (unsigned int i = 0; i < m_rooms.size(); ++i)
	{
		m_rooms.at(i)->Draw();
	}
	testCube->Draw();
}

void LevelHandler::_LoadRoom(const int roomIndex)
{
	Room * room = new Room(roomIndex);

	//Insert assets
	room->LoadRoomFromFile("");


}

void LevelHandler::UnloadRoom(const int roomNumber)
{
	for (unsigned int i = 0; i < m_rooms.size(); ++i)
	{
		if (m_rooms.at(i)->getRoomIndex() == roomNumber)
		{
			m_rooms.erase(m_rooms.begin() + i);
		}
	}
}
