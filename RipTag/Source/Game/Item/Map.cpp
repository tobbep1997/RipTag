#include "RipTagPCH.h"
#include "Map.h"


Map::Map()
{

}

Map::~Map()
{
	delete m_mesh;
}

void Map::Release()
{
	m_mesh->Release(*RipExtern::g_world);
}

bool Map::DeleteMap()
{
	m_deleteMap = true;
	return true;
}


void Map::Init()
{
	m_mesh = new BaseActor();
	m_mesh->Init(*RipExtern::g_world, e_kinematicBody ,0.5, 0.5, 0.5);
	//m_mesh->setPosition()
	Manager::g_meshManager.loadStaticMesh("PRESSUREPLATE");
	Manager::g_textureManager.loadTextures("PRESSUREPLATE");
	m_mesh->setModel(Manager::g_meshManager.getStaticMesh("PRESSUREPLATE"));
	m_mesh->setTexture(Manager::g_textureManager.getTexture("PRESSUREPLATE"));
	m_mesh->setObjectTag("MAP");
	m_mesh->setUserDataBody(this);

}

void Map::Update(double deltaTime)
{
	if (!m_deleteMap)
	{
		m_mesh->ImGuiTransform(tempPos, tempRot,30);
		m_mesh->setPositionRot(tempPos, tempRot);
	}
	
}

void Map::Draw()
{
	if (!m_deleteMap)
	{
		m_mesh->Draw();
	}
	
}
