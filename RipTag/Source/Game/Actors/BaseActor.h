#pragma once
#include "Actor.h"
#include "../../Physics/Wrapper/PhysicsComponent.h"

class BaseActor :
	public Actor, public PhysicsComponent
{
public:

	BaseActor();
	BaseActor(b3World & world, b3BodyType bodyType = b3BodyType::e_dynamicBody);


	virtual ~BaseActor();

	void Init(b3World& world, b3BodyType bodyType, const float & x = 1, const float & y = 1, const float & z = 1);
	void Init(b3World & world, const ImporterLibrary::CollisionBoxes & collisionBoxes);



	void BeginPlay() override;
	void Update(double deltaTime) override;

	void setPosition(const float & x, const float & y, const float & z, const bool & setPhysics = true);
	void setPosition(const DirectX::XMFLOAT4A& pos, const bool setPhysics);
	void setPositionRot(const float & x, const float & y, const float & z, const float & pitch, const float & yaw, const float & roll);
	void setRotation(const float & pitch, const float & yaw, const float & roll, const bool & setPhysics = true);
	void setPhysicsRotation(const float & pitch, const float & yaw, const float & roll);
	void setPositionRot(const DirectX::XMFLOAT4A & pos,const DirectX::XMFLOAT4A rot);

	void addRotation(const float & pitch, const float & yaw, const float & roll);
	void ImGuiTransform(DirectX::XMFLOAT4A & pos, DirectX::XMFLOAT4A & rotation, const float & posMove = 10, const float & rot = 180);
	
};

