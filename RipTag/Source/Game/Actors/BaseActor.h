#pragma once
#include "Actor.h"
#include "../../Physics/Wrapper/PhysicsComponent.h"

class BaseActor :
	public Actor, public PhysicsComponent
{
public:
	
	BaseActor();
	BaseActor(b3World & world, b3BodyType bodyType = b3BodyType::e_dynamicBody);
	~BaseActor();

	void Init(b3World& world, b3BodyType bodyType, const float & x, const float & y, const float & z);

	void BeginPlay() override;
	void Update(double deltaTime) override;

	void setPosition(const float & x, const float & y, const float & z);
};