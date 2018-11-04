#include "RipTagPCH.h"
#include "Door.h"


Door::Door() : Triggerble() , BaseActor()
{
}


Door::~Door()
{
}

void Door::Triggerd(double deltaTime)
{
	setPosition(pos1.x, pos1.y, pos1.z);
}

void Door::unTriggerd(double deltaTime)
{
	setPosition(pos2.x, pos2.y, pos2.z);
}

void Door::setPos(DirectX::XMFLOAT4A pos1, DirectX::XMFLOAT4A pos2)
{
	this->pos1 = pos1;
	
	this->pos2 = pos2;
}
