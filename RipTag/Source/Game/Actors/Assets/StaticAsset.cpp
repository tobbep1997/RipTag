#include "StaticAsset.h"


StaticAsset::StaticAsset() : Actor(), PhysicsComponent()
{
}

StaticAsset::~StaticAsset()
{
}

void StaticAsset::Init(b3World& world, const MyLibrary::CollisionBoxes& collisionBoxes)
{
	PhysicsComponent::Init(world, collisionBoxes);
}

void StaticAsset::Init(b3World& world, const float& x, const float& y, const float& z)
{
	PhysicsComponent::Init(world,e_staticBody,x,y,z);
}

void StaticAsset::BeginPlay()
{
}

void StaticAsset::Update(double deltaTime)
{
}

void StaticAsset::setPosition(const float& x, const float& y, const float& z)
{
	Transform::setPosition(x, y, z);
}


