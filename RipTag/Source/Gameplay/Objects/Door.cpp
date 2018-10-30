#include "Door.h"
#include <iostream>


Door::Door() : Triggerble()
{
}


Door::~Door()
{
}

void Door::Triggerd(double deltaTime)
{
	std::cout << "Femenist" << std::endl;
}

void Door::unTriggerd(double deltaTime)
{
	std::cout << "Vanlig person" << std::endl;

}
