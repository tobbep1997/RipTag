#pragma once
#include "../../Physics/Wrapper/PhysicsComponent.h"
//#include <string>
class RayCastListener : public b3RayCastListener
{	
public:
	struct RayContact
	{
		RayContact(b3Body* originBody, bool singleUse = true)
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

		~RayContact()
		{
			originBody = nullptr;
			contactShape = nullptr;
			delete consumeState;
			consumeState = nullptr;
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
	bool m_rayHit = false;

private:
	virtual r32 ReportShape(b3Shape* shape, const b3Vec3& point, const b3Vec3& normal, r32 fraction)
	{
		if (fraction != 0)
		{
			rayContacts.push_back(new RayContact(m_tempBody, shape, point, normal, fraction, m_singleUse));
			m_rayHit = true;
		}
		return fraction;
	}

public:
	RayCastListener() { }

	virtual ~RayCastListener() 
	{
		ClearQueue();
		m_tempBody = nullptr;
	}

	virtual void ClearQueue()
	{
		for (int i = 0; i < rayContacts.size(); i++)
		{
			delete rayContacts.at(i);
		}
		rayContacts.clear();
	}

	virtual void ClearConsumedContacts()
	{
		if (!rayContacts.empty())
		{
			for (int i = 0; i < rayContacts.size(); i++)
			{
				if (*rayContacts[i]->consumeState >= 2 || *rayContacts[i]->consumeState == 0) //fully used or not used at all
				{
					delete rayContacts.at(i);
					rayContacts.erase(rayContacts.begin() + i);
				}
			}
		}
	}

	virtual RayContact* ShotRay(b3Body* body, DirectX::XMFLOAT4A start, DirectX::XMFLOAT4A direction, float length, bool singleUse = true)
	{
		RayContact* contact = nullptr;
		this->m_tempBody = body;
		this->m_singleUse = singleUse;

		float x = start.x + (length * direction.x);
		float y = start.y + (length * direction.y);
		float z = start.z + (length * direction.z);

		body->GetScene()->RayCast(this, b3Vec3(start.x, start.y, start.z), b3Vec3(x, y, z));

		if (m_rayHit)
		{
			contact = rayContacts.back();
		}

		this->m_tempBody = nullptr;
		this->m_singleUse = true;
		this->m_rayHit = false;
		return contact;
	}

	virtual std::vector<RayContact*> GetContacts()
	{
		return this->rayContacts;
	}

};