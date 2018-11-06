#pragma once
#include "../../Game/Actors/BaseActor.h"

class Triggerable : public BaseActor
{
private:
	int m_uniqueID;
	int m_linkedID;
	bool m_isActivated;
	std::string activatedAnimation;
	std::string deactivatedAnimation;
public:
	Triggerable();
	Triggerable(int uniqueId, int linkedID, bool isTrigger, std::string activeAnim, std::string deactivAnim);
	virtual ~Triggerable();

	//virtual void Triggerd(double deltaTime) = 0;
	//virtual void unTriggerd(double deltaTime) = 0;

	int getLinkId() { return this->m_linkedID; };
	int getUniqueID() { return this->m_uniqueID; };
	bool getState() { return this->m_isActivated; };
	void setState(bool state);

	void BeginPlay() override;
	void Update(double deltaTime) override;
	
	void Release();

};

