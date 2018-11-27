#include "RipTagPCH.h"
#include "ContactListener.h"


ContactListener::ContactListener()
{
	endContacts.reserve(MAX_CONTACTS);
	endContacts.resize(MAX_CONTACTS);
	startContacts.reserve(MAX_CONTACTS);
	startContacts.resize(MAX_CONTACTS);
}


ContactListener::~ContactListener()
{
	startContacts.clear();
	endContacts.clear();
}

void ContactListener::BeginContact(b3Contact* contact)
{
	startContacts.at(m_nrOfBeginContacts).Copy(contact);
	m_nrOfBeginContacts++;
}
void ContactListener::EndContact(b3Contact* contact)
{
	endContacts.at(m_nrOfEndContacts).Copy(contact);
	m_nrOfEndContacts++;
}
void ContactListener::Persisting(b3Contact* contact)
{

}

ContactListener::S_Contact ContactListener::GetBeginContact(unsigned int pos)
{
	return startContacts.at(pos);
}

ContactListener::S_Contact ContactListener::GetEndContact(unsigned int pos)
{
	return endContacts.at(pos);
}


unsigned int ContactListener::GetNrOfEndContacts()
{
	return m_nrOfEndContacts;
}

unsigned int ContactListener::GetNrOfBeginContacts()
{
	return m_nrOfBeginContacts;
}

void ContactListener::ClearContactQueue()
{
	for (int i = 0; i < (int)m_nrOfBeginContacts; i++)
	{
		startContacts.at(i).Empty();
	}
	m_nrOfBeginContacts = 0;

	for (int i = 0; i < (int)m_nrOfEndContacts; i++)
	{
		endContacts.at(i).Empty();
	}
	m_nrOfEndContacts = 0;
}
