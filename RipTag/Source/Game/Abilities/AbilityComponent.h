#pragma once

class AbilityComponent
{
private:
	float m_manaCost;
protected:
	void * p_owner;

public:
	AbilityComponent(void * owner = nullptr);
	virtual ~AbilityComponent();

	virtual void setOwner(void * owner);

	virtual void setManaCost(float mana);
	virtual float getManaCost() const;

	virtual void Init() = 0;
	virtual void Update(double deltaTime) = 0;
	virtual void Use() = 0;
	virtual void Draw() = 0;
};