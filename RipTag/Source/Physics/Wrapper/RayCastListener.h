#pragma once
#include "../../Physics/Wrapper/PhysicsComponent.h"
#include "EngineSource\Helper\Timer.h"
class RayCastListener : public b3RayCastListener
{	
public:
	struct Ray;

	struct RayContact
	{
		friend class RayCastListener;
		b3Shape* contactShape;
		b3Vec3 contactPoint;
		b3Vec3 normal;
		r32 fraction = 0;
		bool free = true;
		Ray* ray;

		RayContact()
		{
			contactShape = nullptr;
			contactPoint.SetZero();
			normal.SetZero();
			fraction = 0;
			ray = nullptr;
		};

		~RayContact()
		{
			contactShape = nullptr;
			ray = nullptr;
		};
	private:
		void _clearGarbage()
		{
			this->contactShape = nullptr;
			this->ray = nullptr;
			this->contactPoint.SetZero();
			this->normal.SetZero();
			this->fraction = 0;
			this->free = true;
		}
		void _setData(Ray* ray, b3Shape* contactShape, const b3Vec3& point, const b3Vec3& normal, r32 fraction)
		{
			this->ray = ray;
			this->contactShape = contactShape;
			this->contactPoint = point;	
			this->normal = normal;
			this->fraction = fraction;
			this->free = false;
		};
		
	};

	struct Ray
	{
	public:
		static const int MAX_CONTACTS = 50;
	private:
		friend class RayCastListener;
		unsigned int id = INT_MAX;
		std::vector<RayContact> rayContacts;
		b3Body* originBody = nullptr;
		bool free = true;
		b3Vec3 startPos = {};
		b3Vec3 endPos = {};
		unsigned int m_nrOfContacts = 0;

	public:

		Ray()
		{
			rayContacts.resize(MAX_CONTACTS);
			
		};
		~Ray()
		{
			rayContacts.clear();
		}

		std::vector<RayContact>& GetRayContacts()
		{
			return this->rayContacts;
		}

		RayContact& GetRayContact(int pos)
		{
			return this->rayContacts.at(pos);
		}

		RayContact& getClosestContact(bool objectPriority = false)
		{
			if (m_nrOfContacts == 0)
				return this->rayContacts.at(MAX_CONTACTS-1);

			float smallestFraction = FLT_MAX;
			unsigned int id = 0;
			for (int i = 0; i < m_nrOfContacts; i++)
			{
				if (this->rayContacts.at(i).fraction <= smallestFraction)
				{
					if (objectPriority && this->rayContacts.at(i).fraction == smallestFraction)
					{
						if (this->rayContacts.at(i).contactShape->GetBody()->GetObjectTag() != "NULL"
							&& this->rayContacts.at(i).contactShape->GetBody()->GetObjectTag() != "WORLD")
						{
							id = i;
							smallestFraction = this->rayContacts.at(i).fraction;
						}
					}
					else
					{
						id = i;
						smallestFraction = this->rayContacts.at(i).fraction;
					}
				}
			}

			return this->rayContacts.at(id);
		}

		int getNrOfContacts()
		{
			return this->m_nrOfContacts;
		}

		b3Body* getOriginBody()
		{
			return this->originBody;
		}

		const bool isFree()
		{
			return this->free;
		}

	private:

		void _setupRay(b3Body* origin, b3Vec3 start, b3Vec3 end)
		{
			this->originBody = origin;
			this->startPos = start;
			this->endPos = end;
			this->free = false;
		}

		bool _addRayContact(b3Shape* contactShape, const b3Vec3& point, const b3Vec3& normal, r32 fraction)
		{
			for (int i = 0; i < MAX_CONTACTS; i++)
			{
				if (rayContacts.at(i).free)
				{
					rayContacts.at(i)._setData(this, contactShape, point, normal, fraction);
					this->m_nrOfContacts++;
					return true;
				}
			}
			return false; //Too many contacts
		};

		void _clearGarbage()
		{
			for (int i = 0; i < MAX_CONTACTS; i++)
			{
				if (!rayContacts[i].free)
				{
					rayContacts[i]._clearGarbage();
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
	static const int MAX_RAYS = 500;
private:
	std::vector<Ray> rays;
	std::vector<Ray> processedRays;
	Ray audioRay;
	unsigned int m_nrOfRays;
	unsigned int m_nrOfProcessedRays;
	b3Body* m_tempBody;
	unsigned int tempID = -1;

private:
	//Called by the physics engine to inform of the shapes intersecting
	virtual r32 ReportShape(b3Shape* shape, const b3Vec3& point, const b3Vec3& normal, r32 fraction)
	{
		if (tempID != -1)
		{
			if (rays.at(tempID).getOriginBody() != shape->GetBody())
			{
				if (!shape->GetBody()->FindInFilters(rays.at(tempID).getOriginBody()->GetObjectTag())
					&& !rays.at(tempID).getOriginBody()->FindInFilters(shape->GetBody()->GetObjectTag()))
					processedRays.at(tempID)._addRayContact(shape, point, normal, fraction);	
			}
		}
		else
		{
			if (audioRay.getOriginBody() != shape->GetBody())
			{
				audioRay._addRayContact(shape, point, normal, fraction);
			}
		}
		return fraction;
	}

public:
	RayCastListener() 
	{ 
		rays.resize(MAX_RAYS);
		processedRays.resize(MAX_RAYS);
		for (int i = 0; i < MAX_RAYS; i++)
		{
			rays.at(i).id = i;
		}
	}

	virtual ~RayCastListener() 
	{
		ClearRays();
		ClearProcessedRays();
		m_tempBody = nullptr;
		rays.clear();
		processedRays.clear();
	}

	virtual void ClearRays()
	{
		for (int i = 0; i < MAX_RAYS; i++)
		{
			if (!rays.at(i).isFree())
			{
				rays.at(i)._clearGarbage();
			}
		}
		m_nrOfRays = 0;
	}

	virtual void ClearProcessedRays()
	{
		for (int i = 0; i < MAX_RAYS; i++)
		{
			if (!processedRays.at(i).isFree())
			{
				processedRays.at(i)._clearGarbage();
				processedRays.at(i).id = -1;
			}
		}
		m_nrOfProcessedRays = 0;
	}

	virtual std::vector<Ray> GetRays()
	{
		return this->rays;
	}

	virtual std::vector<Ray> GetProcessedRays()
	{
		return this->processedRays;
	}

	virtual Ray& GetProcessedRay(unsigned int pos)
	{
		return this->processedRays.at(pos);
	}

	virtual Ray& ConsumeProcessedRay(int& pos)
	{
		const int rPos = pos;
		pos = -100;
		return this->processedRays.at(rPos);
	}

	virtual const unsigned int getNrOfProcessedRays()
	{
		return this->m_nrOfProcessedRays;
	}

	virtual const unsigned int getNrOfRays()
	{
		return this->m_nrOfRays;
	}
	virtual const bool hasRayHit(int &id)
	{
		bool result = true;
		if (id < 0)
			result = false;
		else if (this->processedRays.at(id).getNrOfContacts() == 0)
		{
			id = -100;
			result = false;
		}
		return result;
	}


	virtual void ShootRays()
	{
		ClearProcessedRays();
		Ray ray;
		for (unsigned int i = 0; i < m_nrOfRays; i++)
		{
			ray = rays.at(i);
			tempID = ray.id;
			this->processedRays.at(tempID).id = tempID;
			this->processedRays.at(tempID).free = false;
			this->processedRays.at(tempID).originBody = ray.originBody;
			if(!(ray.startPos.x == ray.endPos.x && ray.startPos.y == ray.endPos.y && ray.startPos.z == ray.endPos.z))
				RipExtern::g_world->RayCast(this, ray.startPos, ray.endPos);
			m_nrOfProcessedRays++;
		}
		tempID = -1;
		ClearRays();
	}

	virtual Ray& ShootAudioRay(b3Body* body, DirectX::XMFLOAT4A start, DirectX::XMFLOAT4A end)
	{
		b3Vec3 startPos(start.x, start.y, start.z);
		b3Vec3 endPos(end.x, end.y, end.z);

		audioRay._clearGarbage();
		audioRay._setupRay(body, startPos, endPos);
		RipExtern::g_world->RayCast(this, startPos, endPos);

		return audioRay;
	}

	virtual unsigned int PrepareRay(b3Body* body, DirectX::XMFLOAT4A start, DirectX::XMFLOAT4A direction, float length)
	{
		b3Vec3 dir;
		dir.x = direction.x;
		dir.y = direction.y;
		dir.z = direction.z;

		if (dir.y == 0)
			dir.y = FLT_EPSILON;

		b3Vec3 startPos(start.x, start.y, start.z);
		b3Vec3 endPos = startPos + (length * dir);

		rays.at(m_nrOfRays)._setupRay(body, startPos, endPos);
		m_nrOfRays++;

		return m_nrOfRays-1;
	}

	virtual unsigned int PrepareRay(b3Body* body, DirectX::XMFLOAT4A start, DirectX::XMFLOAT4A end)
	{
		b3Vec3 startPos(start.x, start.y, start.z);
		b3Vec3 endPos(end.x, end.y, end.z);

		rays.at(m_nrOfRays)._setupRay(body, startPos, endPos);
		m_nrOfRays++;

		return m_nrOfRays - 1;
	}
};