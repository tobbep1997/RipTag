#pragma once
#include "../Actor.h"
#include "../../../Physics/Wrapper/PhysicsComponent.h"

class StaticAsset : public Actor, public PhysicsComponent
{
public:
	StaticAsset();
	~StaticAsset();

	void Init(b3World& world);
};
