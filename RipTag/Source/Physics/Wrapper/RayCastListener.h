#pragma once
#include "../../Physics/Wrapper/PhysicsComponent.h"
#include "EngineSource\Helper\Timer.h"
class RayCastListener : public b3RayCastListener
{	
public:
	struct RayContact
	{
		RayContact()
		{
			contactShape = nullptr;
			contactPoint.SetZero();
			normal.SetZero();
			fraction = 0;
		};

		~RayContact()
		{
			contactShape = nullptr;
		};

		void clearGarbage()
		{
			this->contactShape = nullptr;
			this->contactPoint.SetZero();
			this->normal.SetZero();
			this->fraction = 0;
			this->free = true;
		}
		void setData(b3Shape* contactShape, const b3Vec3& point, const b3Vec3& normal, r32 fraction)
		{
			this->contactShape = contactShape;
			this->contactPoint = point;	
			this->normal = normal;
			this->fraction = fraction;
			this->free = false;
		};

		b3Shape* contactShape;
		b3Vec3 contactPoint;
		b3Vec3 normal;
		r32 fraction = 0;
		bool free = true;
		//0 = unused, 1 = used once, 2 = used twice
		
	};

	struct Ray
	{
	public:
		static const int MAX_CONTACTS = 50;
	private:
		unsigned int id = -1;
		std::vector<RayContact*> rayContacts;
		b3Body* originBody;
		bool free = true;
		b3Vec3 startPos;
		b3Vec3 endPos;
		unsigned int m_nrOfContacts = 0;

	public:

		Ray()
		{
			rayContacts.reserve(MAX_CONTACTS);
			for (int i = 0; i < MAX_CONTACTS; i++)
			{
				rayContacts.push_back(new RayContact());
			}
		};
		~Ray()
		{
			for (int i = 0; i < MAX_CONTACTS; i++)
			{
				delete rayContacts.at(i);
				rayContacts.at(i) = nullptr;
			}
			rayContacts.clear();
		}

		void setupRay(b3Body* origin, b3Vec3 start, b3Vec3 end)
		{
			this->originBody = origin;
			this->startPos = start;
			this->endPos = end;
			this->free = false;
		}

		bool addRayContact(b3Shape* contactShape, const b3Vec3& point, const b3Vec3& normal, r32 fraction)
		{
			for (int i = 0; i < MAX_CONTACTS; i++)
			{
				if (rayContacts.at(i)->free)
				{
					rayContacts.at(i)->setData(contactShape, point, normal, fraction);
					this->free = false;
					this->m_nrOfContacts++;
					return true;
				}
			}
			return false; //Too many contacts
		};

		std::vector<RayContact*> GetRayContacts()
		{
			return this->rayContacts;
		}

		RayContact* GetRayContact(int pos)
		{
			return this->rayContacts.at(pos);
		}

		RayContact* getClosestContact()
		{
			if (m_nrOfContacts == 0)
				return nullptr;
			return this->rayContacts.at(m_nrOfContacts-1);
		}

		int getNrOfContacts()
		{
			return this->m_nrOfContacts;
		}
		const b3Vec3 getStart()
		{
			return this->startPos;
		}
		const b3Vec3 getEnd()
		{
			return this->endPos;
		}
		b3Body* getOriginBody()
		{
			return this->originBody;
		}
		const unsigned int getID()
		{
			return this->id;
		}
		const bool isFree()
		{
			return this->free;
		}

		void setID(unsigned int id)
		{
			this->id = id;
		}
		void setEnd(b3Vec3 end)
		{
			this->endPos = end;
		}

		void setBody(b3Body* body)
		{
			this->originBody = body;
		}

		const bool hasHit()
		{
			return this->m_nrOfContacts != 0;
		}

		void clearGarbage()
		{
			for (int i = 0; i < MAX_CONTACTS; i++)
			{
				if (!rayContacts[i]->free)
				{
					rayContacts[i]->clearGarbage();
				}
			}
			this->originBody = nullptr;
			this->startPos.SetZero();
			this->endPos.SetZero();
			this->m_nrOfContacts = 0;
			this->free = true;
		}
	};
public:
	static const int MAX_RAYS = 50;
private:
	std::vector<Ray*> rays;
	std::vector<Ray*> processedRays;
	unsigned int m_nrOfRays;
	unsigned int m_nrOfProcessedRays;
	b3Body* m_tempBody;
	unsigned int tempID;

private:
	//Called by the physics engine to inform of the shapes intersecting
	virtual r32 ReportShape(b3Shape* shape, const b3Vec3& point, const b3Vec3& normal, r32 fraction)
	{
		if (fraction != 0)
		{
			processedRays.at(tempID)->addRayContact(shape, point, normal, fraction);
		}
		return fraction;
	}

public:
	RayCastListener() 
	{ 
		rays.reserve(MAX_RAYS);
		for (int i = 0; i < MAX_RAYS; i++)
		{
			rays.push_back(new Ray());
			rays.at(i)->setID(i);
			processedRays.push_back(new Ray());
		}
	}

	virtual ~RayCastListener() 
	{
		ClearRays();
		ClearProcessedRays();
		m_tempBody = nullptr;
		for (int i = 0; i < MAX_RAYS; i++)
		{
			delete rays.at(i);
			delete processedRays.at(i);
			rays.at(i) = nullptr;
			processedRays.at(i) = nullptr;
		}
		rays.clear();
		processedRays.clear();
	}

	virtual void ClearRays()
	{
		for (int i = 0; i < MAX_RAYS; i++)
		{
			if (!rays.at(i)->isFree())
			{
				rays.at(i)->clearGarbage();
			}
		}
		m_nrOfRays = 0;
	}

	virtual void ClearProcessedRays()
	{
		for (int i = 0; i < MAX_RAYS; i++)
		{
			if (!processedRays.at(i)->isFree())
			{
				processedRays.at(i)->clearGarbage();
				processedRays.at(i)->setID(-1);
			}
		}
		m_nrOfProcessedRays = 0;
	}

	virtual std::vector<Ray*> GetRays()
	{
		return this->rays;
	}

	virtual std::vector<Ray*> GetProcessedRays()
	{
		return this->processedRays;
	}

	virtual Ray* GetProcessedRay(unsigned int pos)
	{
		return this->processedRays.at(pos);
	}

	virtual const unsigned int getNrOfProcessedRays()
	{
		return this->m_nrOfProcessedRays;
	}

	virtual const unsigned int getNrOfRays()
	{
		return this->m_nrOfRays;
	}
	virtual const bool hasRayHit(int id)
	{
		if (id < 0)
		{
			return false;
		}
		return this->processedRays.at(id)->getNrOfContacts() != 0;
	}


	virtual void ShotRays()
	{
		ClearProcessedRays();
		Ray* ray;
		for (int i = 0; i < m_nrOfRays; i++)
		{
			ray = rays.at(i);
			tempID = ray->getID();
			this->processedRays.at(tempID)->setID(tempID);
			this->processedRays.at(tempID)->setBody(ray->getOriginBody());
			RipExtern::g_world->RayCast(this, ray->getStart(), ray->getEnd());
			m_nrOfProcessedRays++;
		}
		ClearRays();
	}

	virtual unsigned int PrepareRay(b3Body* body, DirectX::XMFLOAT4A start, DirectX::XMFLOAT4A direction, float length)
	{
		b3Vec3 dir;
		dir.x = direction.x;
		dir.y = direction.y;
		dir.z = direction.z;

		b3Vec3 startPos(start.x, start.y, start.z);
		b3Vec3 endPos = startPos + (length * dir);

		bool identical = false;
		b3Vec3 dirOld;
		b3Vec3 dirOldN;
		b3Vec3 startOld;
		unsigned int id = -1;
		int identicalPos = -1;
		int firstFreeSlot = -1;
		/*for (int i = 0; i < MAX_RAYS; i++)
		{
			if (rays.at(i)->isFree() && firstFreeSlot == -1)
				firstFreeSlot = i;

			startOld = rays.at(i)->getStart();
			dirOld = startOld - rays.at(i)->getEnd();
			dirOldN = b3Normalize(dirOld);

			if (!rays.at(i)->isFree() && !identical &&(dir.x == dirOldN.x && dir.y == dirOldN.y && dir.z == dirOldN.z) && (start.x == startOld.x && start.y == startOld.y && start.z == startOld.z))
			{
				identical = true;
				id = rays.at(i)->getID();
				if (b3Len(dirOld) < length)
				{
					rays.at(i)->setEnd(endPos);
				}
			}
			
			if (identical && firstFreeSlot != -1)
				break;
		}

		if (!identical && firstFreeSlot != -1)
		{*/
			rays.at(m_nrOfRays)->setupRay(body, startPos, endPos);
			//id = rays.at(firstFreeSlot)->getID();
			m_nrOfRays++;
		//}

		return m_nrOfRays-1;
	}

	
};