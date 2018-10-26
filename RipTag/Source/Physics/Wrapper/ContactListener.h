#pragma once
#include "../../Physics/Wrapper/PhysicsComponent.h"
class ContactListener : public b3ContactListener
{
private:

public:
	ContactListener();
	virtual ~ContactListener();
	virtual void BeginContact(b3Contact* contact);
	virtual void EndContact(b3Contact* contact);
	virtual void Persisting(b3Contact* contact);
};

