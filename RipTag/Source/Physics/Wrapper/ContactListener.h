#pragma once
#include "../../Physics/Wrapper/PhysicsComponent.h"
class ContactListener : public b3ContactListener
{
private:

	std::vector<b3Contact*> m_beginContacts;
	std::vector<b3Contact*> m_endContacts;
	std::vector<b3Contact*> m_persistingContacts;
	std::vector<b3Shape*> m_endShapesA;
	std::vector<b3Shape*> m_endShapesB;

public:
	ContactListener();
	virtual ~ContactListener();
	virtual void BeginContact(b3Contact* contact);
	virtual void EndContact(b3Contact* contact);
	virtual void Persisting(b3Contact* contact);

	virtual std::vector<b3Contact*> GetBeginContacts();
	virtual std::vector<b3Contact*> GetEndContacts();
	virtual std::vector<b3Contact*> GetPersistingContacts();
	virtual std::vector<b3Shape*> GetEndShapesA();
	virtual std::vector<b3Shape*> GetEndShapesB();

	void ClearContactQueue();
};

