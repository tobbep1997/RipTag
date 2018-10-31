#include "Door.h"
#include <iostream>


Door::Door() : Triggerble() , Drawable()
{
}


Door::~Door()
{
}

void Door::Triggerd(double deltaTime)
{
	setPosition(0, 0, 0);
}

void Door::unTriggerd(double deltaTime)
{
	setPosition(0, 2, 0);
}
