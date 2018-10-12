#pragma once
#include "../Actor.h"
#include "../../../Physics/Wrapper/PhysicsComponent.h"
#include <vector>
class StaticAsset : public Actor, public PhysicsComponent
{
public:
	StaticAsset();
	~StaticAsset();

	void Init(b3World& world, std::vector<CollisionObject> boxes);

	void setPosition(const float& x, const float& y, const float& z);
};
