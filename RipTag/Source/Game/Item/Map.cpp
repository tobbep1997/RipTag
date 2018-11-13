#include "RipTagPCH.h"
#include "Map.h"


Map::Map()
{

}

Map::~Map()
{
}

void Map::Init()
{
	m_mesh->Init(*RipExtern::g_world, e_kinematicBody ,0.2, 0.2, 0.2);
	//m_mesh->setPosition()
	m_mesh->setModel(Manager::g_meshManager.getStaticMesh("FOOT"));
	m_mesh->setTexture(Manager::g_textureManager.getTexture("FOOT"));
}

void Map::Update(double deltaTime)
{
	m_mesh->ImGuiTransform(tempPos, tempRot);
	m_mesh->setPositionRot(tempPos, tempRot);
}

void Map::Draw()
{
	m_mesh->Draw();
}
