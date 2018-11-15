#include "Engine2DPCH.h"
#include "HUDComponent.h"
#include <fstream>


HUDComponent::HUDComponent()
{

}

HUDComponent::~HUDComponent()
{
	for (unsigned int i = 0; i < m_quads.size(); i++)
	{
		m_quads[i]->quad->Release();
		delete m_quads[i]->quad;
		delete m_quads[i];
	}
}


void HUDComponent::InitHUDFromFile(std::string fileName)
{
	std::ifstream inputStream;
	inputStream.open(fileName);

	if (inputStream.is_open())
	{
		std::string keyWords[] = {"i","pp","b","text","f","c","texture","t" };
		std::string collector = "";
		unsigned int type = 0; 

		int nrOfElements = 0;
		int sepIndex = 0;
		Quad* currQuad; 

		inputStream >> nrOfElements;
		inputStream.ignore();

		for (int i = 0; i < nrOfElements; i++)
		{
			std::getline(inputStream, collector);
			
			type = std::stoi(collector); 

			if (type == 0)
				currQuad = new Quad();
			else
				currQuad = new Circle(); 

			std::getline(inputStream, collector);

			//Init
			if (collector == keyWords[0])
			{
				float posX;
				float posY;
				float sizeX; 
				float sizeY; 
				int midPoint; 

				std::getline(inputStream, collector);

				sepIndex = collector.find_first_of(',');
				posX = std::stof(collector.substr(1, sepIndex - 1));
				posY = std::stof(collector.substr(sepIndex + 1));

				midPoint = collector.find(';'); 
				sepIndex = collector.find_last_of(',');
				sizeX = std::stof(collector.substr(midPoint + 1, sepIndex - 1)); 
				sizeY = std::stof(collector.substr(sepIndex + 1));

				if(type == 0)
				currQuad->init(DirectX::XMFLOAT2A(posX, posY), DirectX::XMFLOAT2A(sizeX, sizeY));
				else
				{
					float radius; 
					sepIndex = collector.find_last_of(';');
					radius = std::stof(collector.substr(sepIndex + 1)); 

					currQuad->init(DirectX::XMFLOAT2A(posX, posY), DirectX::XMFLOAT2A(sizeX, sizeY));
					dynamic_cast<Circle*>(currQuad)->setRadie(radius); 
				}
			}

			std::getline(inputStream, collector);

			//PivotPoint 
			if (collector == keyWords[1])
			{
				int pivotPointType;

				inputStream >> pivotPointType;
				inputStream.ignore();

				Quad::PivotPoint pivotPoint = static_cast<Quad::PivotPoint>(pivotPointType);

				currQuad->setPivotPoint(pivotPoint);
			}

			std::getline(inputStream, collector);

			//Butt or no Butt Bitch
			if (collector == keyWords[2])
			{
				int button;
				inputStream >> button;
				inputStream.ignore();

				currQuad->setIsButton(button);
			}

			std::getline(inputStream, collector);

			//Text
			if (collector == keyWords[3])
			{
				std::getline(inputStream, collector);
				if (collector == "NULL")
					currQuad->setString(""); 
				else
					currQuad->setString(collector);
			}

			std::getline(inputStream, collector);

			//Font 
			if (collector == keyWords[4])
			{
				inputStream >> collector;
				inputStream.ignore();

				FontHandler::loadFont(collector);

				currQuad->setFont(FontHandler::getFont(collector));
			}

			std::getline(inputStream, collector);

			//TextColor
			if (collector == keyWords[5])
			{
				int rPos, gPos, bPos, aPos;
				float r, g, b, a;

				inputStream >> collector;
				inputStream.ignore();

				rPos = collector.find('r');
				gPos = collector.find('g');
				bPos = collector.find('b');
				aPos = collector.find('a');

				r = std::stof(collector.substr(rPos + 1, 5));
				g = std::stof(collector.substr(gPos + 1, 5));
				b = std::stof(collector.substr(bPos + 1, 5));
				a = std::stof(collector.substr(aPos + 1, 5));

				currQuad->setTextColor(DirectX::XMFLOAT4A(r, g, b, a));
			}

			std::getline(inputStream, collector);

			//Texture
			if (collector == keyWords[6])
			{
				int pressPos, unPressPos, hoverPos;

				std::string textureNames[3] = { "", "", "" };

				inputStream >> collector;
				inputStream.ignore();

				pressPos = collector.find('p');
				unPressPos = collector.find('u');
				hoverPos = collector.find('h');

				textureNames[0] = collector.substr(pressPos + 1, collector.find(',') - 1);
				textureNames[1] = collector.substr(unPressPos + 1, (collector.find_last_of(',') - (textureNames[0].size() + 3)));
				textureNames[2] = collector.substr(hoverPos + 1);

				Manager::g_textureManager.loadTextures(textureNames[0].c_str());
				Manager::g_textureManager.loadTextures(textureNames[1].c_str());
				Manager::g_textureManager.loadTextures(textureNames[2].c_str());

				currQuad->setPressedTexture(textureNames[0].c_str());
				currQuad->setUnpressedTexture(textureNames[1].c_str());
				currQuad->setHoverTexture(textureNames[2].c_str());
			}

			std::getline(inputStream, collector); 

			//Tag
			if (collector == keyWords[7])
			{
				inputStream >> collector; 
				inputStream.ignore(); 

				currQuad->setTag(collector); 
			}
			AddQuad(currQuad);
		}

	}
}

void HUDComponent::AddQuad(Quad *& quad, unsigned int keyCode)
{
	QUAD_OBJECT * qo = new QUAD_OBJECT();
	qo->quad = quad;
	qo->keyCode = keyCode;
	m_quads.push_back(qo);
}

Quad * HUDComponent::GetQuad(const unsigned short int & i)
{
	if (i < m_quads.size())
		return m_quads[i]->quad;
	else
		throw "u stoopid";
}

Quad * HUDComponent::GetQuad(std::string tag)
{
	for (int i = 0; i < m_quads.size(); i++)
	{
		if (m_quads[i]->quad->getTag() == tag)
		{
			return m_quads[i]->quad; 
		}
	}
}

void HUDComponent::removeHUD()
{
	for (int i = 0; i < m_quads.size(); i++)
	{
		m_quads[i]->quad->Release(); 
		delete m_quads[i]->quad;
		delete m_quads[i];
	}
	m_quads.clear(); 
}

void HUDComponent::ResetStates()
{
	for (unsigned int i = 0; i < m_quads.size(); i++)
	{
		m_quads[i]->quad->setState(0);
	}
}

void HUDComponent::setSelectedQuad(const unsigned short int & i)
{
	if (i < m_quads.size())
		m_quads[i]->quad->setState(2);
	else
		throw "u stoopid";
}

void HUDComponent::HUDUpdate(double deltaTime)
{
}

void HUDComponent::HUDDraw()
{
	for (unsigned int i = 0; i < m_quads.size(); i++)
	{
		m_quads[i]->quad->Draw();
	}
}
