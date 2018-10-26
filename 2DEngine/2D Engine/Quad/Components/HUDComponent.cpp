#include "HUDComponent.h"
#include "InputManager/InputHandler.h"


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

void HUDComponent::HUDUpdate(double deltaTime)
{
	for (unsigned int i = 0; i < m_quads.size(); i++)
	{
		m_quads[i]->quad->setState(0);
		if (InputHandler::isKeyPressed(m_quads[i]->keyCode) && m_quads[i]->keyCode != 0x0)		
			m_currentTarget = i;
	}
	m_quads[m_currentTarget]->quad->setState(2);

	
}

void HUDComponent::HUDDraw()
{
	for (unsigned int i = 0; i < m_quads.size(); i++)
	{
		m_quads[i]->quad->Draw();
	}
}
