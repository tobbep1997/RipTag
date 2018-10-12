#include "PhaseAction.h"



PhaseAction::PhaseAction()
{
	this->m_rayListener = new RayCastListener();
}

b3Vec3 PhaseAction::Phase(b3Body* body, DirectX::XMFLOAT4A direction, float length)
{
	b3Vec3 pos;

	float x = body->GetTransform().translation.x + (length * direction.x);
	float y = body->GetTransform().translation.y + (length * direction.y);
	float z = body->GetTransform().translation.z + (length * direction.z);
	//b3Vec3(-1.5,2.1, -2.1)
	body->GetScene()->RayCast(this->m_rayListener,
		b3Vec3(body->GetTransform().translation.x, body->GetTransform().translation.y, body->GetTransform().translation.z),
		b3Vec3(x, y, z));
	if (this->m_rayListener->fraction != 0)
	{
		pos.x = this->m_rayListener->contactPoint.x + (0.5*(-this->m_rayListener->normal.x));
		pos.y = body->GetTransform().translation.y;
		pos.z =	this->m_rayListener->contactPoint.z + (0.5*(-this->m_rayListener->normal.z));
	}
	else
	{
		pos = b3Vec3(0, 0, 0);
	}
	this->m_rayListener->clear();
	return pos;
}


PhaseAction::~PhaseAction()
{
	delete this->m_rayListener;
}
