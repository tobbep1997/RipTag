#include "ContactListener.h"
#include <iostream>


ContactListener::ContactListener()
{
}


ContactListener::~ContactListener()
{
}

void ContactListener::BeginContact(b3Contact* contact)
{

	void* bodyUserDataA = contact->GetShapeA()->GetBody()->GetUserData();
	void* bodyUserDataB = contact->GetShapeB()->GetBody()->GetUserData();
	//static_cast<Entity*>(bodyUserDataA)->handleContact(bodyUserDataA, bodyUserDataB, contact->isTouching())
	//static_cast<Entity*>(bodyUserDataB)->handleContact(bodyUserDataB, bodyUserDataA, contact->isTouching())
}
void ContactListener::EndContact(b3Contact* contact)
{
	void* bodyUserDataA = contact->GetShapeA()->GetBody()->GetUserData();
	void* bodyUserDataB = contact->GetShapeB()->GetBody()->GetUserData();
	//static_cast<Entity*>(bodyUserDataA)->handleContact(bodyUserDataA, bodyUserDataB, contact->isTouching())
	//static_cast<Entity*>(bodyUserDataB)->handleContact(bodyUserDataB, bodyUserDataA, contact->isTouching())
}
void ContactListener::Persisting(b3Contact* contact)
{
	void* bodyUserDataA = contact->GetShapeA()->GetBody()->GetUserData();
	void* bodyUserDataB = contact->GetShapeB()->GetBody()->GetUserData();
	//static_cast<Entity*>(bodyUserDataA)->handleContact(bodyUserDataA, bodyUserDataB, contact->isTouching())
	//static_cast<Entity*>(bodyUserDataB)->handleContact(bodyUserDataB, bodyUserDataA, contact->isTouching())
}
