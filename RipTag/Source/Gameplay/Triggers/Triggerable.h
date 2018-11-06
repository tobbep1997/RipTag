#pragma once
#include "../../Game/Actors/BaseActor.h"

class Triggerable : public BaseActor
{
private:
	int m_uniqueID;
	int m_linkedID;
	bool m_isTrigger;
public:
	Triggerable();
	Triggerable(int uniqueId, int linkedID, bool isTrigger);
	virtual ~Triggerable();

	//virtual void Triggerd(double deltaTime) = 0;
	//virtual void unTriggerd(double deltaTime) = 0;

	int getLinkId() { return this->m_linkedID; };
	int getUniqueID() { return this->m_uniqueID; };
	bool getIsTriggerable() { return this->m_isTrigger; };

	void BeginPlay() override;
	void Update(double deltaTime) override;
	
	void Release();

};

