#include "PhaseAction.h"


PhaseAction::PhaseAction()
{
}

b3Vec3 PhaseAction::Phase(RayCastListener *rayListener)
{
	b3Vec3 pos = b3Vec3(0, 0, 0);
	if (rayListener->shape != nullptr)
	{
		pos.x = rayListener->contactPoint.x + (0.5*(-rayListener->normal.x));
		pos.y = rayListener->contactPoint.y + (0.5*(-rayListener->normal.y));
		pos.z = rayListener->contactPoint.z + (0.5*(-rayListener->normal.z));
	}
	return pos;
}


PhaseAction::~PhaseAction()
{

}
