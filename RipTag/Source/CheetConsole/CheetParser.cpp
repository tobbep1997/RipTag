#include "RipTagPCH.h"
#include "CheetParser.h"
Player * CheetParser::playerPtr = nullptr;
EnemyHandler * CheetParser::enemyHandlerPtr = nullptr;
CheetParser * CheetParser::GetInstance()
{
	
	static CheetParser m_instance;
	return &m_instance;
	
}

void CheetParser::ParseString(const std::string& input)
{
	//std::transform(input.begin(), input.end(), input.begin(), ::tolower);
	if (input == "player")
	{
		_PlayerCheets();
	}
	else if (input == "*noclip*")
	{
		
	}
	else if (input == "lose")
	{
		PlayState::setYouLost(true);
	}
	else if (input == "win")
	{
		playerPtr->InstaWin();
	}
	else if (input == "spawn")
	{
		_SpawnCheets();
	}
	
}

bool CheetParser::SetPlayerCheetPointer(Player * p)
{
	playerPtr = p;
	return true;
}

bool CheetParser::SetEnemyHandlerCheetPointer(EnemyHandler* en)
{
	enemyHandlerPtr = en;
	return true;
}

void CheetParser::_PlayerCheets()
{
	int x = -1;
	system("CLS");
	std::cout << "Player Commands" << std::endl;
	std::cout << "1: Get Position" << std::endl;
	std::cout << "2: Set Position" << std::endl;
	std::cout << "3: Jump to first guard" << std::endl;

	int posX = 0;
	int posY = 0;
	int posZ = 0;
	std::cin >> x;
	switch (x)
	{
	case(1):

		posX = playerPtr->getPosition().x;
		posY = playerPtr->getPosition().y;
		posZ = playerPtr->getPosition().z;
		std::cout << "Player Pos X:" << posX << " Y:" << posY << " Z:" << posZ << std::endl;
		break;
	case(2):
		std::cout << "Enter X" << std::endl;
		std::cin >> posX;
		std::cout << "Enter Y" << std::endl;
		std::cin >> posY;
		std::cout << "Enter Z" << std::endl;
		std::cin >> posZ;

		playerPtr->setPosition(posX, posY, posZ);
		break;
	case(3):
		Enemy * temp = enemyHandlerPtr->GetFirstEnemy();
		playerPtr->setPosition(temp->getPosition());
		break;

	
	}
}

void CheetParser::_SpawnCheets()
{
	int x = -1;
	system("CLS");
	std::cout << "Spawn Commands" << std::endl;
	std::cout << "1: Guard" << std::endl;
	//std::cout << "2: Set Position" << std::endl;

	int posX = 0;
	int posY = 0;
	int posZ = 0;
	std::cin >> x;
	switch (x)
	{
	case(0):
		
		break;

	}
}
