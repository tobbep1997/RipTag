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
	setPosition(-13.046, -3.832, 5.68698);
}

void Door::unTriggerd(double deltaTime)
{
	setPosition(-10.046, -3.832, 5.68698);
}
