#pragma once
#include "../../Physics/Wrapper/PhysicsComponent.h"
#include <iostream>
class RayCastListener : public b3RayCastListener
{	
public:
	b3Vec3 contactPoint;
	b3Vec3 normal;
	r32 fraction = 0;
	b3Shape* shape;
	int type = -1;

	virtual r32 ReportShape(b3Shape* shape, const b3Vec3& point, const b3Vec3& normal, r32 fraction)
	{
		if (fraction != 0)
		{
			this->shape = shape;
			this->contactPoint = point;
			this->normal = normal;
			this->fraction = fraction;
			this->type = (int)shape->GetUserData();
		}
		return fraction;
	}

	void clear()
	{
		this->shape = nullptr;
		this->contactPoint = b3Vec3(0, 0, 0);
		this->normal = b3Vec3(0, 0, 0);
		this->fraction = 0;
		this->type = -1;
	}

	void shotRay(b3Body* body, DirectX::XMFLOAT4A direction, float length)
	{
		//b3Vec3 pos;

		float x = body->GetTransform().translation.x + (length * direction.x);
		float y = body->GetTransform().translation.y + (length * direction.y);
		float z = body->GetTransform().translation.z + (length * direction.z);
		//b3Vec3(-1.5,2.1, -2.1)
		body->GetScene()->RayCast(this,
			b3Vec3(body->GetTransform().translation.x, body->GetTransform().translation.y, body->GetTransform().translation.z),
			b3Vec3(x, y, z));
	}

	virtual ~RayCastListener() { }
};