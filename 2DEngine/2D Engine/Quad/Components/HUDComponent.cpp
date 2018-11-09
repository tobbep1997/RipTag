#include "Engine2DPCH.h"
#include "HUDComponent.h"

HUDComponent::HUDComponent()
{
}


HUDComponent::~HUDComponent()
{
	for (unsigned int i = 0; i < m_quads.size(); i++)
	{
		m_quads[i]->quad->Release();
		delete m_quads[i]->quad;
		delete m_quads[i];
	}
}

void HUDComponent::AddQuad(Quad *& quad, unsigned int keyCode)
{
	QUAD_OBJECT * qo = new QUAD_OBJECT();
	qo->quad = quad;
	qo->keyCode = keyCode;
	m_quads.push_back(qo);
}

void HUDComponent::ResetStates()
{
	for (unsigned int i = 0; i < m_quads.size(); i++)
	{
		m_quads[i]->quad->setState(0);
	}
}

void HUDComponent::setSelectedQuad(const unsigned short int & i)
{
	if (i < m_quads.size())
		m_quads[i]->quad->setState(2);
	else
		throw "u stoopid";
}

void HUDComponent::HUDUpdate(double deltaTime)
{
}

void HUDComponent::HUDDraw()
{
	for (unsigned int i = 0; i < m_quads.size(); i++)
	{
		m_quads[i]->quad->Draw();
	}
}
