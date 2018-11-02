#include "Door.h"
#include <iostream>


Door::Door() : Triggerble() , BaseActor()
{
}


Door::~Door()
{
}

void Door::Triggerd(double deltaTime)
{
	setPosition(-13.046f, -3.832f, 5.68698f);
}

void Door::unTriggerd(double deltaTime)
{
	setPosition(-10.046f, -3.832f, 5.68698f);
}
