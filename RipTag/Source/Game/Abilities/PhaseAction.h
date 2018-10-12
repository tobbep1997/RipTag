#pragma once
#include "../../Physics/Wrapper/PhysicsComponent.h"
#include <DirectXMath.h>
class PhaseAction
{
private:
	struct RayCastListener : public b3RayCastListener
	{
		b3Vec3 contactPoint;
		b3Vec3 normal;
		r32 fraction;
		b3Shape* shape;

		virtual ~RayCastListener() { }
		virtual r32 ReportShape(b3Shape* shape, const b3Vec3& point, const b3Vec3& normal, r32 fraction)
		{
			if (fraction != 0)
			{
				this->shape = shape;
				this->contactPoint = point;
				this->normal = normal;
				this->fraction = fraction;
			}

			return fraction;
		}
		void clear()
		{
			this->contactPoint = b3Vec3(0, 0, 0);
			this->normal = b3Vec3(0, 0, 0);
			this->fraction = 0;
			this->shape = nullptr;
		}
	};

	RayCastListener *m_rayListener;

public:
	PhaseAction();
	b3Vec3 Phase(b3Body* body, DirectX::XMFLOAT4A direction, float length);
	virtual ~PhaseAction();
};

