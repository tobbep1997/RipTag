#include "RipTagPCH.h"
#include "CheetParser.h"
Player * CheetParser::playerPtr = nullptr;
CheetParser * CheetParser::GetInstance()
{
	
	static CheetParser m_instance;
	return &m_instance;
	
}

void CheetParser::ParseString(const std::string& input)
{
	if (input == "Player")
	{
		int x = -1;
		std::cout << "Player Commands" << std::endl;
		std::cout << "1: Get Position" << std::endl;
		std::cout << "2: Set Position" << std::endl;

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

		}
		//int x = 0, y = 0, z = 0;
		//std::cout << "Enter position x,y,z" << std::endl;
		//std::cin >> x;
		//std::cin >> y;
		//std::cin >> z;
		//playerPtr->setPosition(x, y, z);
	}
}

bool CheetParser::SetPlayerCheetPointer(Player * p)
{
	playerPtr = p;
	return true;
}
