#pragma once
#include "AbilityComponent.h"
#include "EngineSource/3D Engine/Components/Base/Drawable.h"


class VisabilityAbility : public AbilityComponent
{
	Drawable * m_visSphere;
	bool m_isUsing = false;
public:
	VisabilityAbility();
	~VisabilityAbility();

	// Inherited via AbilityComponent
	virtual void Init() override;
	virtual void Update(double deltaTime) override;
	virtual void Use() override;
	virtual void Draw() override;
};

