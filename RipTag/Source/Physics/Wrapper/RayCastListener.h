#pragma once
#include "../../Physics/Wrapper/PhysicsComponent.h"
#include <string>
//#include <iostream>
class RayCastListener : public b3RayCastListener
{	
public:
	struct RayContact
	{
		RayContact(b3Body* originBody, bool singleUse = false)
		{
			this->originBody = originBody;
			contactShape = nullptr;
			contactPoint.SetZero();
			normal.SetZero();
			fraction = 0;
			consumeState = new int(0);
			if(singleUse)
			*consumeState = 2;
		};
		~RayContact()
		{
			originBody = nullptr;
			contactShape = nullptr;
			delete consumeState;
			consumeState = nullptr;
		};

		RayContact(b3Body* originBody, b3Shape* contactShape, const b3Vec3& point, const b3Vec3& normal, r32 fraction, bool singleUse = true)
		{
			this->originBody = originBody;

			this->contactShape = contactShape;
			this->contactPoint = point;
			this->normal = normal;
			this->fraction = fraction;

			consumeState = new int(0);
			if (singleUse)
				*consumeState = 2;
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
		int *consumeState;
		//0 = unused, 1 = used once, 2 = used twice
		
	};
private:
	std::vector<RayContact*> rayContacts;
	b3Body* m_tempBody;
	bool m_singleUse = false;

public:
	virtual r32 ReportShape(b3Shape* shape, const b3Vec3& point, const b3Vec3& normal, r32 fraction)
	{
		if (fraction != 0)
		{
			rayContacts.push_back(new RayContact(m_tempBody, shape, point, normal, fraction, m_singleUse));
			/*if (rayContacts.back().contactShape->IsSensor() && *rayContacts.back().consumeState != 2)
			{
				rayContacts.push_back(RayContact(rayContacts.back().originBody));
			}*/
		}
		return fraction;
	}

	virtual void ClearQueue()
	{
		rayContacts.clear();
	}

	virtual void ClearConsumedContacts()
	{
		if (!rayContacts.empty())
		{
			for (int i = 0; i < rayContacts.size(); i++)
			{
				if (*rayContacts[i]->consumeState >= 2)
				{
					delete rayContacts.at(i)->consumeState;
					rayContacts.erase(rayContacts.begin() + i);
				}
			}
		}
	}

	virtual RayContact* ShotRay(b3Body* body, DirectX::XMFLOAT4A start, DirectX::XMFLOAT4A direction, float length, bool singleUse = true)
	{
		//b3Vec3 pos;
		RayContact* contact = nullptr;
		this->m_tempBody = body;
		this->m_singleUse = singleUse;
		float x = start.x + (length * direction.x);
		float y = start.y + (length * direction.y);
		float z = start.z + (length * direction.z);

		body->GetScene()->RayCast(this, b3Vec3(start.x, start.y, start.z), b3Vec3(x, y, z));

		if (!rayContacts.empty())
		{
			if (rayContacts.back()->fraction == 0)
			{
				rayContacts.pop_back();
			}
			else if (singleUse)
			{
				contact = rayContacts.back();
			}
		}

		this->m_tempBody = nullptr;
		this->m_singleUse = true;
		return contact;
	}

	virtual std::vector<RayContact*> GetContacts()
	{
		return this->rayContacts;
	}

	RayCastListener() { }
	virtual ~RayCastListener() 
	{
		for (int i = (int)rayContacts.size()-1; i > 0; i--)
		{
			rayContacts.erase(rayContacts.begin() + i);
		}
	}
};