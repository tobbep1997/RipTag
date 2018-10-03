#include "BaseActor.h"



BaseActor::BaseActor() : Actor(), PhysicsComponent()
{
}

BaseActor::BaseActor(b3World& world, b3BodyType bodyType) : Actor(), PhysicsComponent()
{
	this->Init(world, bodyType);
	
}


BaseActor::~BaseActor()
{
}

void BaseActor::BeginPlay()
{

}

void BaseActor::Update(double deltaTime)
{
	p_updatePhysics(this);
}

void BaseActor::setPosition(const float& x, const float& y, const float& z)
{
	Transform::setPosition(x, y, z);
	PhysicsComponent::p_setPosition(x, y, z);
}


