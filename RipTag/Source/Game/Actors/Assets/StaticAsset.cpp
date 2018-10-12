#include "StaticAsset.h"


StaticAsset::StaticAsset() : Actor(), PhysicsComponent()
{
}

StaticAsset::~StaticAsset()
{
}

void StaticAsset::Init(b3World& world, std::vector<CollisionObject> boxes)
{
	PhysicsComponent::Init(world, boxes);
}

void StaticAsset::setPosition(const float& x, const float& y, const float& z)
{
	Transform::setPosition(x, y, z);
}


