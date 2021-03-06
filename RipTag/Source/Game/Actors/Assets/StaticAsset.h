#pragma once
#include "../Actor.h"
#include "../../../Physics/Wrapper/PhysicsComponent.h"
#include <vector>
class StaticAsset : public Actor, public PhysicsComponent
{
public:
	StaticAsset();
	~StaticAsset();

	void Init(b3World& world, const ImporterLibrary::CollisionBoxes & collisionBoxes);
	void Init(b3World& world, const float& x, const float& y, const float& z);

	void BeginPlay() override;
	void Update(double deltaTime) override;

	void setPosition(const float& x, const float& y, const float& z);
};
