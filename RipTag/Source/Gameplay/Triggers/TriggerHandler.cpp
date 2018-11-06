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
	for (size_t i = 0; i < Triggers.size(); i++)
		Triggers[i]->Update(deltaTime);

	for (size_t i = 0; i < Triggerables.size(); i++)
		Triggerables[i]->Update(deltaTime);

	for (auto &pair : localTriggerPairs)
	{
		if (pair.second.triggerables.size() > 0)
		{
			if (!pair.second.triggerables[0]->getState())
			{
				//If not active check the linked triggers state - if all are true set all triggerable's to true
				for (size_t i = 0; i < pair.second.triggers.size(); i++)
				{
					if (!pair.second.triggers[i]->getTriggerState())
						break;
					if ((i + 1) == pair.second.triggers.size())
					{
						for (size_t j = 0; j < pair.second.triggerables.size(); j++)
							pair.second.triggerables[j]->setState(true);
					}
				}
			}
		}
		//check the first Triggerable object and see if it is not Active
	}
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

void TriggerHandler::HandlePacket(unsigned char id, unsigned char * data)
{
	Network::TRIGGEREVENTPACKET * _data = (Network::TRIGGEREVENTPACKET*)data;
	netWorkTriggers.at(_data->uniqueID)->setTriggerState(_data->state);
}

void TriggerHandler::RegisterThisInstanceToNetwork()
{
	using namespace Network;
	using namespace std::placeholders;

	Multiplayer::addToOnReceiveFuncMap(NETWORKMESSAGES::ID_TRIGGER_USED, std::bind(&TriggerHandler::HandlePacket, this, _1, _2));
}
