#pragma once
class Triggerble
{
public:
	Triggerble();
	virtual~Triggerble();

	virtual void Triggerd(double deltaTime) = 0;
	virtual void unTriggerd(double deltaTime) = 0;
};

