#include "RipTagPCH.h"
#include "CheatParser.h"
Player * CheatParser::playerPtr = nullptr;
EnemyHandler * CheatParser::enemyHandlerPtr = nullptr;

bool CheatParser::m_visabilityDisable = false;
bool CheatParser::m_DrawCollisionBoxes = false;

bool CheatParser::m_DBG_CAM = false;
CheatParser * CheatParser::GetInstance()
{
	
	static CheatParser m_instance;
	return &m_instance;
	
}

void CheatParser::ParseString(const std::string& input)
{
	//std::transform(input.begin(), input.end(), input.begin(), ::tolower);
	if (input == "player")
	{
		_PlayerCheats();
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
		//_SpawnCheats();
	}
	else if (input == "common")
	{
		_CommonCheats();
	}
	else if (input == "engine")
	{
		_EngineCheats();
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

bool CheatParser::SetPlayerCheatPointer(Player * p)
{
	playerPtr = p;
	return true;
}

bool CheatParser::SetEnemyHandlerCheatPointer(EnemyHandler* en)
{
	enemyHandlerPtr = en;
	return true;
}

bool CheatParser::GetVisabilityDisabled()
{
	return m_visabilityDisable;
}

bool CheatParser::GetDrawCollisionBoxes()
{
	return m_DrawCollisionBoxes;
}

bool CheatParser::GetDBG_CAM()
{
	return m_DBG_CAM;
}

void CheatParser::_PlayerCheats()
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

void CheatParser::_SpawnCheats()
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

void CheatParser::_CommonCheats()
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
		std::cout << "Visability state " << CheatParser::m_visabilityDisable << std::endl;
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

void CheatParser::_EngineCheats()
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
		std::cout << "Debug Camera state " << CheatParser::m_DBG_CAM << std::endl;
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

void CheatParser::_Draw()
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
		std::cout << "CollisionBoxes state " << CheatParser::m_DrawCollisionBoxes << std::endl;
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

void CheatParser::_Help()
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
