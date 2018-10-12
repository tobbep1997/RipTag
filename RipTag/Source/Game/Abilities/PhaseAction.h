#pragma once
#include "../../Physics/Wrapper/PhysicsComponent.h"
#include "../../Physics/Wrapper/RayCastListener.h"

class PhaseAction
{
private:

public:
	PhaseAction();
	b3Vec3 Phase(RayCastListener *rayListener);
	virtual ~PhaseAction();
};

