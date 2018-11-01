#include "ContactListener.h"
#include <iostream>
#include "../../Game/Actors/Player.h"


ContactListener::ContactListener()
{
}


ContactListener::~ContactListener()
{
}

void ContactListener::BeginContact(b3Contact* contact)
{
	m_beginContacts.push_back(contact);
}
void ContactListener::EndContact(b3Contact* contact)
{
	m_endContacts.push_back(S_EndContact(contact));
	m_endShapesA.push_back(contact->GetShapeA());
	m_endShapesB.push_back(contact->GetShapeB());
}
void ContactListener::Persisting(b3Contact* contact)
{
	m_persistingContacts.push_back(contact);
}

std::vector<b3Contact*> ContactListener::GetBeginContacts()
{
	return m_beginContacts;
}

std::vector<ContactListener::S_EndContact> ContactListener::GetEndContacts()
{
	return m_endContacts;
}


std::vector<b3Contact*> ContactListener::GetPersistingContacts()
{
	return m_persistingContacts;
}

std::vector<b3Shape*> ContactListener::GetEndShapesA()
{
	return m_endShapesA;
}

std::vector<b3Shape*> ContactListener::GetEndShapesB()
{
	return m_endShapesB;
}

void ContactListener::ClearContactQueue()
{
	m_beginContacts.clear();
	m_endContacts.clear();
	m_persistingContacts.clear();
	m_endShapesA.clear();
	m_endShapesB.clear();
}
