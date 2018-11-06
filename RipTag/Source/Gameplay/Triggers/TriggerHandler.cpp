#include "RipTagPCH.h"
#include "TriggerHandler.h"



TriggerHandler::TriggerHandler()
{
	
}


TriggerHandler::~TriggerHandler()
{
	
	netWorkTriggers.clear();
	localTriggerPairs.clear();
	for (size_t i = 0; i < Triggers.size(); i++)
		delete Triggers[i];
	for (size_t i = 0; i < Triggerables.size(); i++)
		delete Triggerables[i];
}

void TriggerHandler::Update(double deltaTime)
{	
	
}

void TriggerHandler::Draw()
{
	for (size_t i = 0; i < Triggers.size(); i++)
		Triggers[i]->Draw();
	for (size_t i = 0; i < Triggerables.size(); i++)
		Triggerables[i]->Draw();
}


void TriggerHandler::Release()
{
	for (size_t i = 0; i < Triggers.size(); i++)
		Triggers[i]->Release();
	for (size_t i = 0; i < Triggerables.size(); i++)
		Triggerables[i]->Release();
}

void TriggerHandler::LoadTriggerPairMap()
{
	for (int i = 0; i <this->Triggerables.size(); i++)
	{
		int tempLink = this->Triggerables[i]->getLinkId();

		std::map<int, TriggerHandler::TriggerPair>::iterator it = this->localTriggerPairs.find(tempLink);
		if (it != this->localTriggerPairs.end())
		{
			it->second.triggerables.push_back(this->Triggerables[i]);
		}
		else
		{
			TriggerHandler::TriggerPair t_pair;
			t_pair.triggerables.push_back(this->Triggerables[i]);
			this->localTriggerPairs.insert(std::pair<int, TriggerHandler::TriggerPair>(tempLink, t_pair));
		}
	}
	for (int j = 0; j < this->Triggers.size(); j++)
	{
		this->localTriggerPairs[this->Triggers[j]->getLinkId()].triggers.push_back(this->Triggers[j]);// (this->Triggers[j]->getLinkId()).triggers.push_back(this->Triggers[j]);

		//this->localTriggerPairs.at(this->Triggers[j]->getLinkId()).triggers.push_back(this->Triggers[j]);
	}
}
