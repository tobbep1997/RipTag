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

	if (getAnimationPlayer())
		getAnimationPlayer()->Update(deltaTime);
}

void BaseActor::setPosition(const DirectX::XMFLOAT4A& pos, const bool setPhysics)
{
	setPosition(pos.x, pos.y, pos.z, setPhysics);
}

void BaseActor::setPosition(const float& x, const float& y, const float& z, const bool & setPhysics)
{
	Transform::setPosition(x, y, z);
	if (setPhysics)
		PhysicsComponent::p_setPosition(x, y, z);
}

void BaseActor::setPositionRot(const float& x, const float& y, const float& z, const float& pitch, const float& yaw,
	const float& roll)
{
	Transform::setPosition(x, y, z);
	Transform::setRotation(DirectX::XMConvertToRadians(pitch), DirectX::XMConvertToRadians(yaw), DirectX::XMConvertToRadians(roll));
	PhysicsComponent::p_setPositionRot(x, y, z, DirectX::XMConvertToRadians(pitch), DirectX::XMConvertToRadians(yaw), DirectX::XMConvertToRadians(roll));
}

void BaseActor::setPositionRot(const DirectX::XMFLOAT4A& pos, const DirectX::XMFLOAT4A rot)
{
	setPositionRot(pos.x, pos.y, pos.z, rot.x, rot.y, rot.z);
}
void BaseActor::setRotation(const float& pitch, const float& yaw, const float& roll, const bool & setPhysics)
{
	Transform::setRotation(DirectX::XMConvertToRadians(pitch), DirectX::XMConvertToRadians(yaw), DirectX::XMConvertToRadians(roll));
	if (setPhysics)
		PhysicsComponent::p_setRotation(DirectX::XMConvertToRadians(pitch), DirectX::XMConvertToRadians(yaw), DirectX::XMConvertToRadians(roll));
}

void BaseActor::setPhysicsRotation(const float & pitch, const float & yaw, const float & roll)
{
	PhysicsComponent::p_setRotation(DirectX::XMConvertToRadians(pitch), DirectX::XMConvertToRadians(yaw), DirectX::XMConvertToRadians(roll));

}

void BaseActor::addRotation(const float& pitch, const float& yaw, const float& roll)
{
	Transform::addRotation(DirectX::XMConvertToRadians(pitch), DirectX::XMConvertToRadians(yaw), DirectX::XMConvertToRadians(roll));
	PhysicsComponent::p_addRotation(DirectX::XMConvertToRadians(pitch), DirectX::XMConvertToRadians(yaw), DirectX::XMConvertToRadians(roll));
}

void BaseActor::ImGuiTransform(DirectX::XMFLOAT4A& pos, DirectX::XMFLOAT4A & rotation, const float& posMove, const float& rot)
{
	ImGui::Begin("Cube");
	ImGui::SliderFloat("PositionX", &pos.x, -posMove, posMove);
	ImGui::SliderFloat("PositionY", &pos.y, -posMove, posMove);
	ImGui::SliderFloat("PositionZ", &pos.z, -posMove, posMove);

	ImGui::SliderFloat("DirX", &rotation.x, -rot, rot);
	ImGui::SliderFloat("DirY", &rotation.y, -rot, rot);
	ImGui::SliderFloat("DirZ", &rotation.z, -rot, rot);

	ImGui::End();
}






