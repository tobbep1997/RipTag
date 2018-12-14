#pragma once
#include "../../Physics/Wrapper/PhysicsComponent.h"
class ContactListener : public b3ContactListener
{
public:
	struct S_EndContact
	{
		S_EndContact(b3Contact * contact)
		{
			a = contact->GetShapeA();
			b = contact->GetShapeB();
		}
		b3Shape * a = nullptr;
		b3Shape * b = nullptr;

	};

	struct S_Contact
	{
		S_Contact()
		{
			a = nullptr;
			b = nullptr;
			free = true;
		}
		S_Contact(b3Contact * contact)
		{
			a = contact->GetShapeA();
			b = contact->GetShapeB();
			free = false;
		}

		void Copy(b3Contact * contact)
		{
			a = contact->GetShapeA();
			b = contact->GetShapeB();
			free = false;
		}

		void Empty()
		{
			a = nullptr;
			b = nullptr;
			free = true;
		}
		b3Shape * a = nullptr;
		b3Shape * b = nullptr;
		bool free = true;
	};

private:
	const int MAX_CONTACTS = 100;
	unsigned int m_nrOfBeginContacts;
	unsigned int m_nrOfEndContacts;
	std::vector<S_Contact> startContacts;
	std::vector<S_Contact> endContacts;

public:
	ContactListener();
	virtual ~ContactListener();

	virtual void BeginContact(b3Contact* contact);
	virtual void EndContact(b3Contact* contact);
	virtual void Persisting(b3Contact* contact);

	S_Contact GetBeginContact(unsigned int pos);
	S_Contact GetEndContact(unsigned int pos);

	unsigned int GetNrOfEndContacts();
	unsigned int GetNrOfBeginContacts();

	void ClearContactQueue();
};

