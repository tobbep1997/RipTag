#pragma once
#include "../../Physics/Wrapper/PhysicsComponent.h"
#include <string>
//#include <iostream>
class RayCastListener : public b3RayCastListener
{	
public:
	struct RayContact
	{
		RayContact(b3Body* originBody)
		{
			this->originBody = originBody;
		};
		void _setData(b3Shape* contactShape, const b3Vec3& point, const b3Vec3& normal, r32 fraction)
		{
			this->contactShape = contactShape;
			this->contactPoint = point;
			this->normal = normal;
			this->fraction = fraction;
		};
		b3Body* originBody;
		b3Shape* contactShape;
		b3Vec3 contactPoint;
		b3Vec3 normal;
		r32 fraction = 0;
		
	};
private:
	std::vector<RayContact> rayContacts;

public:
	virtual r32 ReportShape(b3Shape* shape, const b3Vec3& point, const b3Vec3& normal, r32 fraction)
	{
		if (fraction != 0 && !rayContacts.empty())
		{
			rayContacts.back()._setData(shape, point, normal, fraction);
			if (rayContacts.back().contactShape->IsSensor())
			{
				rayContacts.push_back(RayContact(rayContacts.back().originBody));
			}
		}
		return fraction;
	}

	virtual void ClearQueue()
	{
		rayContacts.clear();
	}

	virtual void ShotRay(b3Body* body, DirectX::XMFLOAT4A direction, float length, std::string target = "N/A")
	{
		//b3Vec3 pos;
		float x = body->GetTransform().translation.x + (length * direction.x);
		float y = body->GetTransform().translation.y + (length * direction.y);
		float z = body->GetTransform().translation.z + (length * direction.z);

		rayContacts.push_back(RayContact(body));
		body->GetScene()->RayCast(this,
			b3Vec3(body->GetTransform().translation.x, body->GetTransform().translation.y, body->GetTransform().translation.z),
			b3Vec3(x, y, z));


		if (rayContacts.back().fraction == 0)
		{
			rayContacts.pop_back();
		}
		else if (target != "N/A" && rayContacts.back().contactShape->GetBody()->GetObjectTag() != target)
		{
			rayContacts.pop_back();
		}
		//if (!rayContacts.empty())
		//{
		//	if (!rayContacts.back().contactShape->GetBody()->GetObjectTag().empty())
		//	{
		//		std::cout << rayContacts.back().contactShape->GetBody()->GetObjectTag() << std::endl;
		//	}
		//}
	}

	virtual std::vector<RayContact> GetContacts()
	{
		return this->rayContacts;
	}

	RayCastListener() { }
	virtual ~RayCastListener() { }
};