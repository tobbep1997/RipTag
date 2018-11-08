#include "RipTagPCH.h"
#include "BaseActor.h"



BaseActor::BaseActor() : Actor(), PhysicsComponent()
{
}

BaseActor::BaseActor(b3World& world, b3BodyType bodyType) : Actor(), PhysicsComponent()
{
	
	
}


BaseActor::~BaseActor()
{
	
}

void BaseActor::Init(b3World& world, b3BodyType bodyType, const float & x, const float & y, const float & z)
{
	PhysicsComponent::Init(world, bodyType, x, y, z);
}

void BaseActor::Init(b3World & world, const ImporterLibrary::CollisionBoxes & collisionBoxes)
{
	PhysicsComponent::Init(world, collisionBoxes);
}


void BaseActor::BeginPlay()
{

}

void BaseActor::Update(double deltaTime)
{
	p_updatePhysics(this);
}

void BaseActor::InitPositionRot(const float& x, const float& y, const float& z, const float& pitch, const float& yaw,
	const float& roll)
{
	Transform::setPosition(x, y, z);
	Transform::setRotation(DirectX::XMConvertToRadians(pitch), DirectX::XMConvertToRadians(yaw), DirectX::XMConvertToRadians(roll));
	PhysicsComponent::p_setPositionRot(x, y, z, DirectX::XMConvertToRadians(pitch), DirectX::XMConvertToRadians(yaw), DirectX::XMConvertToRadians(roll));

}

void BaseActor::setPosition(const float& x, const float& y, const float& z)
{
	Transform::setPosition(x, y, z);
	PhysicsComponent::p_setPosition(x, y, z);
}

void BaseActor::setPositionRot(const float& x, const float& y, const float& z, const float& pitch, const float& yaw,
	const float& roll)
{
	Transform::setPosition(x, y, z);
	Transform::setRotation(DirectX::XMConvertToRadians(pitch), DirectX::XMConvertToRadians(yaw), DirectX::XMConvertToRadians(roll));
	PhysicsComponent::p_setPositionRot(x, y, z, DirectX::XMConvertToRadians(pitch), DirectX::XMConvertToRadians(yaw), DirectX::XMConvertToRadians(roll));
	//PhysicsComponent::p_setPositionRot(x, y, z, DirectX::XMConvertToRadians(-pitch), DirectX::XMConvertToRadians(-yaw), DirectX::XMConvertToRadians(-roll));
}

void BaseActor::setRotation(const float& pitch, const float& yaw, const float& roll)
{
	Transform::setRotation(DirectX::XMConvertToRadians(pitch), DirectX::XMConvertToRadians(yaw), DirectX::XMConvertToRadians(roll));
	PhysicsComponent::p_setRotation(DirectX::XMConvertToRadians(pitch), DirectX::XMConvertToRadians(yaw), DirectX::XMConvertToRadians(roll));
}

void BaseActor::addRotation(const float& pitch, const float& yaw, const float& roll)
{

}




