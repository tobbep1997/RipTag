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

	Triggers.clear();
	Triggerables.clear();
}

void TriggerHandler::Update(double deltaTime)
{	
	for (size_t i = 0; i < Triggers.size(); i++)
	{
		Triggers[i]->Update(deltaTime);
	}
		
	for (size_t i = 0; i < Triggerables.size(); i++)
		Triggerables[i]->Update(deltaTime);

	for (auto &pair : localTriggerPairs)
	{
		//if lever or pressure
		for (int i = 0; i < pair.second.triggerables.size(); i++)
		{
			if (pair.second.triggers.size() > 0)
			{
				if (dynamic_cast<PressurePlate*> (pair.second.triggers[0]))
				{
					for (int j = 0; j < pair.second.triggers.size(); j++)
					{
						if (pair.second.triggers[j]->getTriggerState() == false)
						{
						//	pair.second.triggerables[i]->setState(false);
						}
						else
						{
							pair.second.triggerables[i]->setState(true);
							break;
						}
					}
				}
				else
				{
					bool shouldOpen = true;
					for (int j = 0; j < pair.second.triggers.size(); j++)
					{
						if (pair.second.triggers[j]->getTriggerState() == false)
							shouldOpen = false;
						pair.second.triggerables[i]->setState(shouldOpen);

					}
				}
			}	
		}
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
	for (auto & t : Triggers)
	{
		int tmpLink = t->getLinkId();
		std::map<int, TriggerHandler::TriggerPair>::iterator it = this->localTriggerPairs.find(tmpLink);
		if (it != this->localTriggerPairs.end())
		{
			it->second.triggers.push_back(t);
		}
		else
		{
			TriggerPair tp;
			tp.triggers.push_back(t);
			this->localTriggerPairs.insert(std::pair<int, TriggerHandler::TriggerPair>(tmpLink, tp));
		}
	}
	for (auto & pair : localTriggerPairs)
	{
		int tempLink = pair.first;
		for (int i = 0; i < this->Triggerables.size(); i++)
		{
			//tempLink == this->Triggerables[i]->getLinkId();
			if (this->Triggerables[i]->getLinkId() == tempLink)
				pair.second.triggerables.push_back(this->Triggerables[i]);	
		}

	}
		/*std::map<int, TriggerHandler::TriggerPair>::iterator it = this->localTriggerPairs.find(tempLink);
		if (it != this->localTriggerPairs.end())
		{
			it->second.triggerables.push_back(this->Triggerables[i]);
		}
		else
		{
			TriggerHandler::TriggerPair t_pair;
			t_pair.triggerables.push_back(this->Triggerables[i]);
			this->localTriggerPairs.insert(std::pair<int, TriggerHandler::TriggerPair>(tempLink, t_pair));
		}*/
	//for (int j = 0; j < this->Triggers.size(); j++)
	//{
	//	this->localTriggerPairs[this->Triggers[j]->getLinkId()].triggers.push_back(this->Triggers[j]);// (this->Triggers[j]->getLinkId()).triggers.push_back(this->Triggers[j]);

	//	//this->localTriggerPairs.at(this->Triggers[j]->getLinkId()).triggers.push_back(this->Triggers[j]);
	//}
}

void TriggerHandler::HandlePacket(unsigned char id, unsigned char * data)
{
	Network::TRIGGEREVENTPACKET * _data = (Network::TRIGGEREVENTPACKET*)data;
	netWorkTriggers.at(_data->uniqueID)->setTriggerState(_data->state, false);
	//ifnetwork is tru -> false, vice versa
	
}

void TriggerHandler::RegisterThisInstanceToNetwork()
{
	using namespace Network;
	using namespace std::placeholders;

	Multiplayer::addToOnReceiveFuncMap(NETWORKMESSAGES::ID_TRIGGER_USED, std::bind(&TriggerHandler::HandlePacket, this, _1, _2));
}
