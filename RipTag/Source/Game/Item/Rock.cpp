#include "RipTagPCH.h"
#include "Rock.h"

Rock::Rock()
{
}

Rock::~Rock()
{
	delete m_mesh;
}

void Rock::Init()
{
	m_mesh = new BaseActor();
	m_mesh->Init(*RipExtern::g_world, e_staticBody, 0.5, 0.5, 0.5);
	Manager::g_meshManager.loadStaticMesh("PRESSUREPLATE");
	Manager::g_textureManager.loadTextures("PRESSUREPLATE");
	m_mesh->setModel(Manager::g_meshManager.getStaticMesh("PRESSUREPLATE"));
	m_mesh->setTexture(Manager::g_textureManager.getTexture("PRESSUREPLATE"));
	m_mesh->setObjectTag("ROCK_PICKUP");
	m_mesh->setUserDataBody(this);
}

void Rock::Release()
{
	m_mesh->Release(*RipExtern::g_world);
}

void Rock::Update(double deltaTime)
{
	if (!m_deleteRock)
	{
		m_mesh->ImGuiTransform(tempPos, tempRot, 30);
		m_mesh->setPositionRot(tempPos, tempRot);
	}
}

void Rock::Draw()
{
	if (!m_deleteRock)
	{
		m_mesh->Draw();
	}
}

bool Rock::DeleteRock()
{
	m_deleteRock = true;
	return true;
}