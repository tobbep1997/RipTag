#pragma once
#include "../../Physics/Wrapper/PhysicsComponent.h"
#include "EngineSource\Helper\Timer.h"
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

	struct Ray
	{
	private:
		std::vector<RayContact*> rayContacts;
		bool ExistingContacts = true;
	public:
		Ray()
		{
		};
		~Ray()
		{
			ClearQueue();
		}
		void addRayContact(b3Body* originBody, b3Shape* contactShape, const b3Vec3& point, const b3Vec3& normal, r32 fraction, bool singleUse = true)
		{
			rayContacts.push_back(new RayContact(originBody, contactShape, point, normal, fraction, singleUse));
		};

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
					if (*rayContacts[i]->consumeState >= 2 || *rayContacts[i]->consumeState == 0)
					{
						delete rayContacts.at(i);
						rayContacts.erase(rayContacts.begin() + i);
					}
				}
			}
		}

		std::vector<RayContact*> GetRayContacts()
		{
			return this->rayContacts;
		}

		RayContact* getClosestContact()
		{
			return this->rayContacts.back();
		}

		int getNrOfContacts()
		{
			return this->rayContacts.size();
		}
	};
private:
	std::vector<Ray*> rays;
	b3Body* m_tempBody;
	bool m_singleUse = true;
	bool m_rayHit = false;

private:
	//Called by the physics engine to inform of the shapes intersecting
	virtual r32 ReportShape(b3Shape* shape, const b3Vec3& point, const b3Vec3& normal, r32 fraction) 
	{
		if (fraction != 0)
		{
			if (m_rayHit == false)
			{
				rays.push_back(new Ray());
			}
			rays.back()->addRayContact(m_tempBody, shape, point, normal, fraction, m_singleUse);
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
		for (int i = 0; i < rays.size(); i++)
		{
			delete rays.at(i);
		}
		rays.clear();
	}

	//removes contacts that have been accessed once by each object or not used at all
	virtual void ClearConsumedContacts()
	{
		if (!rays.empty())
		{
			for (int i = 0; i < rays.size(); i++)
			{
				rays.at(i)->ClearConsumedContacts();
				if (rays.at(i)->GetRayContacts().empty())
				{
					delete rays.at(i);
					rays.erase(rays.begin() + i);
				}
			}
		}
	}

	/*Prepares the start and end positions of the ray, then calls the physics engine to perform a raycast.
	If specified, the objects being intersected will stay alive until the object shooting the ray and the object being intersected have accessed the contact once.
	contact, will be destroyed if no object has accessed the contact*/
	virtual Ray* ShotRay(b3Body* body, DirectX::XMFLOAT4A start, DirectX::XMFLOAT4A direction, float length, bool singleUse = true)
	{
		Ray* contact = nullptr;
		this->m_tempBody = body;
		this->m_singleUse = singleUse;

		float x = start.x + (length * direction.x);
		float y = start.y + (length * direction.y);
		float z = start.z + (length * direction.z);

		body->GetScene()->RayCast(this, b3Vec3(start.x, start.y, start.z), b3Vec3(x, y, z));
		

		if (m_rayHit)
		{
			contact = rays.back();
		}

		this->m_tempBody = nullptr;
		this->m_singleUse = true;
		this->m_rayHit = false;
		return contact;
	}

	virtual std::vector<Ray*> GetRays()
	{
		return this->rays;
	}

};