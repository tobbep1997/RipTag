#include "RipTagPCH.h"
#include "CheetParser.h"
Player * CheetParser::playerPtr = nullptr;
EnemyHandler * CheetParser::enemyHandlerPtr = nullptr;

bool CheetParser::m_visabilityDisable = false;
bool CheetParser::m_DrawCollisionBoxes = false;

bool CheetParser::m_DBG_CAM = false;
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
		std::cout << "SpawnBroken" << std::endl;
		//_SpawnCheets();
	}
	else if (input == "common")
	{
		_CommonCheets();
	}
	else if (input == "engine")
	{
		_EngineCheets();
	}
	else if (input == "draw")
	{
		_Draw();
	}
	else if (input == "help" || input == "HELP")
	{
		_Help();
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

bool CheetParser::GetVisabilityDisabled()
{
	return m_visabilityDisable;
}

bool CheetParser::GetDrawCollisionBoxes()
{
	return m_DrawCollisionBoxes;
}

bool CheetParser::GetDBG_CAM()
{
	return m_DBG_CAM;
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
	case(1):
		enemyHandlerPtr->SpawnEnemy(playerPtr->getPosition().x, playerPtr->getPosition().y, playerPtr->getPosition().z);
		break;

	}
}

void CheetParser::_CommonCheets()
{
	int x = -1;
	system("CLS");
	std::cout << "Common Commands" << std::endl;
	std::cout << "1: Visability" << std::endl;
	

	
	std::cin >> x;
	switch (x)
	{
	case(1):
		std::string in;
		std::cout << "Visability state " << CheetParser::m_visabilityDisable << std::endl;
		std::cin >> in;
		if (in == "y" || in == "1")
		{
			m_visabilityDisable = true;
		}
		else
		{
			m_visabilityDisable = false;
		}
		break;
	}
}

void CheetParser::_EngineCheets()
{
	int x = -1;
	system("CLS");
	std::cout << "Engine Commands" << std::endl;
	std::cout << "1: Debug Camera" << std::endl;


	std::string in;
	std::cin >> x;
	switch (x)
	{
	case(1):
		std::cout << "Debug Camera state " << CheetParser::m_DBG_CAM << std::endl;
		std::cin >> in;
		if (in == "y" || in == "1")
		{
			m_DBG_CAM = true;
		}
		else
		{
			m_DBG_CAM = false;
		}
		break;
	}
}

void CheetParser::_Draw()
{
	int x = -1;
	system("CLS");
	std::cout << "Draw Commands" << std::endl;
	std::cout << "1: CollisionBoxes" << std::endl;



	std::cin >> x;
	switch (x)
	{
	case(1):
		std::string in;
		std::cout << "CollisionBoxes state " << CheetParser::m_DrawCollisionBoxes << std::endl;
		std::cin >> in;
		if (in == "y" || in == "1")
		{
			m_DrawCollisionBoxes = true;
		}
		else
		{
			m_DrawCollisionBoxes = false;
		}
		break;
	}
}

void CheetParser::_Help()
{
	std::cout << "---HELP---" << std::endl;
	std::cout << "player" << std::endl;
	std::cout << "lose" << std::endl;
	std::cout << "win" << std::endl;
	std::cout << "common" << std::endl;
	std::cout << "spawn" << std::endl;
	std::cout << "draw" << std::endl;

	std::cout << "----------" << std::endl;
}
