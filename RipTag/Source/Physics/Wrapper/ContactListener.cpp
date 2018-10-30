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
	
	void* bodyUserDataA = contact->GetShapeA()->GetBody()->GetUserData();
	void* bodyUserDataB = contact->GetShapeB()->GetBody()->GetUserData();

	//static_cast<Entity*>(bodyUserDataA)->handleContact(bodyUserDataA, bodyUserDataB, contact->isTouching())
	//static_cast<Entity*>(bodyUserDataB)->handleContact(bodyUserDataB, bodyUserDataA, contact->isTouching())
}
void ContactListener::EndContact(b3Contact* contact)
{

	//m_endContacts.push_back(contact);
	m_endShapesA.push_back(contact->GetShapeA());
	m_endShapesB.push_back(contact->GetShapeB());

	void* bodyUserDataA = contact->GetShapeA()->GetBody()->GetUserData();
	void* bodyUserDataB = contact->GetShapeB()->GetBody()->GetUserData();
	
	//static_cast<Entity*>(bodyUserDataA)->handleContact(bodyUserDataA, bodyUserDataB, contact->isTouching())
	//static_cast<Entity*>(bodyUserDataB)->handleContact(bodyUserDataB, bodyUserDataA, contact->isTouching())
}
void ContactListener::Persisting(b3Contact* contact)
{
	m_persistingContacts.push_back(contact);
	void* bodyUserDataA = contact->GetShapeA()->GetBody()->GetUserData();
	void* bodyUserDataB = contact->GetShapeB()->GetBody()->GetUserData();

	
	//static_cast<Entity*>(bodyUserDataA)->handleContact(bodyUserDataA, bodyUserDataB, contact->isTouching())
	//static_cast<Entity*>(bodyUserDataB)->handleContact(bodyUserDataB, bodyUserDataA, contact->isTouching())
}

std::vector<b3Contact*> ContactListener::GetBeginContacts()
{
	return m_beginContacts;
}

std::vector<b3Contact*> ContactListener::GetEndContacts()
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
