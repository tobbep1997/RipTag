#include "RipTagPCH.h"
#include "RoomGenerator.h"

RoomGenerator::RoomGenerator()
{	
}

RoomGenerator::~RoomGenerator()
{
}

void RoomGenerator::dbgFuncSpawnAboveMap()
{
	asset = DBG_NEW BaseActor();
	asset->Init(*m_worldPtr, e_staticBody, m_roomWidth * 2, 5, m_roomDepth * 2);
	//asset->setModel(Manager::g_meshManager.getStaticMesh("FLOOR"));
	//asset->setTexture(Manager::g_textureManager.getTexture("FLOOR"));
	asset->setTextureTileMult(m_roomWidth, m_roomDepth);
	asset->setPosition(0, 10, 0);
	m_generated_assetVector.push_back(asset);
	returnableRoom->setPlayer1StartPos(DirectX::XMFLOAT4(0, 15, 0, 1));
	returnableRoom->setPlayer2StartPos(DirectX::XMFLOAT4(0, 15, 0, 1));

}

void RoomGenerator::applyTransformationToBoundingBox(DirectX::XMMATRIX roomMatrix, ImporterLibrary::CollisionBoxes & boxesToModify)
{
	DirectX::XMVECTOR decomposeTranslation;
	DirectX::XMVECTOR decomposeRotation;
	DirectX::XMVECTOR decomposeScaling;
	for (int k = 0; k < boxesToModify.nrOfBoxes && true; k++)
	{
		DirectX::XMFLOAT3 cPos = DirectX::XMFLOAT3(boxesToModify.boxes[k].translation);
		DirectX::XMFLOAT3 cScl = DirectX::XMFLOAT3(boxesToModify.boxes[k].scale);
		DirectX::XMFLOAT4 cQuat = DirectX::XMFLOAT4(boxesToModify.boxes[k].rotation);

		DirectX::XMMATRIX boxTranslationMatrix = DirectX::XMMatrixTranslation(cPos.x, cPos.y, cPos.z);
		DirectX::XMMATRIX boxRotationMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&cQuat));
		DirectX::XMMATRIX boxScaleMatrix = DirectX::XMMatrixScaling(cScl.x, cScl.y, cScl.z);

		DirectX::XMMATRIX itemWorldSpace = boxScaleMatrix * boxRotationMatrix * boxTranslationMatrix;


		itemWorldSpace = itemWorldSpace * roomMatrix;


		DirectX::XMMatrixDecompose(&decomposeScaling, &decomposeRotation, &decomposeTranslation, itemWorldSpace);

		boxesToModify.boxes[k].translation[0] = DirectX::XMVectorGetX(decomposeTranslation);
		boxesToModify.boxes[k].translation[1] = DirectX::XMVectorGetY(decomposeTranslation);
		boxesToModify.boxes[k].translation[2] = DirectX::XMVectorGetZ(decomposeTranslation);

		boxesToModify.boxes[k].rotation[0] = DirectX::XMVectorGetX(decomposeRotation);
		boxesToModify.boxes[k].rotation[1] = DirectX::XMVectorGetY(decomposeRotation);
		boxesToModify.boxes[k].rotation[2] = DirectX::XMVectorGetZ(decomposeRotation);
		boxesToModify.boxes[k].rotation[3] = DirectX::XMVectorGetW(decomposeRotation);

		boxesToModify.boxes[k].scale[0] = DirectX::XMVectorGetX(decomposeScaling);
		boxesToModify.boxes[k].scale[1] = DirectX::XMVectorGetY(decomposeScaling);
		boxesToModify.boxes[k].scale[2] = DirectX::XMVectorGetZ(decomposeScaling);

	}
}

void RoomGenerator::_generateGrid()
{
	srand(time(NULL));

	m_nrOfWalls = 4; // MAke random
	m_roomDepth = (20 * m_roomGridDepth) / 2.0f;
	m_roomWidth = (20 * m_roomGridWidth) / 2.0f;
	m_generatedGrid = DBG_NEW Grid(-m_roomWidth, -m_roomDepth, m_roomWidth * 2, m_roomDepth * 2);
	
}

void RoomGenerator::_makeFloor()
{
	asset = DBG_NEW BaseActor();
	asset->Init(*m_worldPtr, e_staticBody, m_roomWidth * 2, 0.5, m_roomDepth * 2);
	asset->setModel(Manager::g_meshManager.getStaticMesh("FLOOR"));
	asset->setTexture(Manager::g_textureManager.getTexture("FLOOR"));
	asset->setTextureTileMult(m_roomWidth, m_roomDepth);
	asset->setPosition(0, 0, 0);
	
	asset->setScale(abs(m_roomWidth*2), 1, abs(m_roomDepth* 2));
	m_generated_assetVector.push_back(asset);
}

void RoomGenerator::_makeWalls()
{
	std::vector<ImporterLibrary::GridStruct*> appendedGridStruct;
	int RANDOM_MOD_NR = 0;
	int MAX_SMALL_MODS = 1; // change when small mods added
	bool isRotated = false;

	RandomRoomGrid randomizer;
	ImporterLibrary::CustomFileLoader loader;

	randomizer.GenerateRoomLayout();

	int widthCounter = 0;
	int depthCounter = 0;
	
	ImporterLibrary::CollisionBoxes  modCollisionBoxes;
	modCollisionBoxes.boxes = nullptr;

	Manager::g_meshManager.loadStaticMesh("CLOSEDWALL");
	Manager::g_meshManager.loadStaticMesh("OPENWALL");
	
	int depth = (int)(m_roomDepth + 0.5f);
	int width = (int)(m_roomWidth + 0.5f);
	for (int i = -depth + 10; i <= depth - 10; i += 20)
	{

		widthCounter = 0;
		for (int j = -width + 10; j <= width - 10; j += 20)
		{
			bool directions[4];
			int index = depthCounter * m_roomGridWidth + widthCounter;

			directions[0] = randomizer.m_rooms[index].north;
			directions[1] = randomizer.m_rooms[index].east;
			directions[2] = randomizer.m_rooms[index].south;
			directions[3] = randomizer.m_rooms[index].west;
			

			RANDOM_MOD_NR = MAX_SMALL_MODS;//rand() % 2 + 1; //INCREASE AS MODS INCREASE, MAKE ONE FOR double rooms

			if (randomizer.m_rooms[index].type == 0 || randomizer.m_rooms[index].type == 1)
			{
				RANDOM_MOD_NR = 2;//random from smallmods+1
			}
			isRotated = false;
			if (randomizer.m_rooms[index].type == 1)
				isRotated = true;

			
			std::string MODNAMESTRING = "MOD" + std::to_string(RANDOM_MOD_NR);

#pragma region WALLS
			for (int x = 0; x < 4; x++)
			{
				bool placeWall = true;
				if (randomizer.m_rooms[index].leadsToRoom[x] != -1)
					if (randomizer.m_rooms[index].pairedWith == randomizer.m_rooms[index].leadsToRoom[x])
						placeWall = false;

				if (placeWall)
				{
					asset = DBG_NEW BaseActor();

					if (directions[x])
					{
						asset->setModel(Manager::g_meshManager.getStaticMesh("OPENWALL"));
						modCollisionBoxes = loader.readMeshCollisionBoxes("../Assets/OPENWALLFOLDER/OPENWALL_BBOX.bin");
					}
					else
					{
						asset->setModel(Manager::g_meshManager.getStaticMesh("CLOSEDWALL"));
						modCollisionBoxes = loader.readMeshCollisionBoxes("../Assets/CLOSEDWALLFOLDER/CLOSEDWALL_BBOX.bin");
					}
					asset->setTexture(Manager::g_textureManager.getTexture("WALL"));


					if (x == 0)
					{
						DirectX::XMMATRIX roomSpace = DirectX::XMLoadFloat4x4A(&asset->getWorldmatrix());
						roomSpace = DirectX::XMMatrixTranspose(roomSpace);
						applyTransformationToBoundingBox(roomSpace, modCollisionBoxes);
						asset->Init(*m_worldPtr, modCollisionBoxes);
						asset->setPosition(j, 2.5, i - 10.f);
						m_generated_assetVector.push_back(asset);
					}
					if (x == 1)
					{
						asset->setRotation(0, 90, 0, false);
						DirectX::XMMATRIX roomSpace = DirectX::XMLoadFloat4x4A(&asset->getWorldmatrix());
						roomSpace = DirectX::XMMatrixTranspose(roomSpace);
						applyTransformationToBoundingBox(roomSpace, modCollisionBoxes);
						asset->Init(*m_worldPtr, modCollisionBoxes);
						asset->setPosition(j + 10.f, 2.5, i);
						m_generated_assetVector.push_back(asset);
					}
					if (x == 2)
					{
						DirectX::XMMATRIX roomSpace = DirectX::XMLoadFloat4x4A(&asset->getWorldmatrix());
						roomSpace = DirectX::XMMatrixTranspose(roomSpace);
						applyTransformationToBoundingBox(roomSpace, modCollisionBoxes);
						asset->Init(*m_worldPtr, modCollisionBoxes);
						asset->setPosition(j, 2.5, i + 10.f);
						m_generated_assetVector.push_back(asset);
					}

					if (x == 3)
					{
						asset->setRotation(0, 90, 0, false);
						DirectX::XMMATRIX roomSpace = DirectX::XMLoadFloat4x4A(&asset->getWorldmatrix());
						roomSpace = DirectX::XMMatrixTranspose(roomSpace);
						applyTransformationToBoundingBox(roomSpace, modCollisionBoxes);
						asset->Init(*m_worldPtr, modCollisionBoxes);
						asset->setPosition(j - 10.f, 2.5, i);
						m_generated_assetVector.push_back(asset);
					}
					if (modCollisionBoxes.boxes)
						delete[] modCollisionBoxes.boxes;
				}
			}

#pragma endregion

			int BigRoomAddX = 0;
			int BigRoomAddZ = 0;

			int startX = -m_roomWidth + 10;
			int startZ = -m_roomDepth + 10;
			if (RANDOM_MOD_NR > MAX_SMALL_MODS)
			{
				if (!isRotated)
					BigRoomAddX = 10;
				else
					BigRoomAddZ = 10;

			}
#pragma region GRID

			ImporterLibrary::GridStruct * tempGridStruct;

			tempGridStruct = loader.readGridFile(MODNAMESTRING);

		
			for (int a = 0; a < tempGridStruct->maxY; a++)
			{
				if (isRotated == false)
				{
					for (int b = 0; b < tempGridStruct->maxX; b++)
					{
						tempGridStruct->gridPoints[a + b * tempGridStruct->maxY].translation[0] += j + BigRoomAddX;
						//appendedGridStruct[i]->gridPoints[a + b * appendedGridStruct[i]->maxY].translation[0] += i;
						tempGridStruct->gridPoints[a + b * tempGridStruct->maxY].translation[2] += i + BigRoomAddZ;
					}
				}
				else
				{
					for (int b = 0; b < tempGridStruct->maxX; b++)
					{
						tempGridStruct->gridPoints[a + b * tempGridStruct->maxY].translation[0] += i + BigRoomAddX;
						//appendedGridStruct[i]->gridPoints[a + b * appendedGridStruct[i]->maxY].translation[0] += i;
						tempGridStruct->gridPoints[a + b * tempGridStruct->maxY].translation[2] += j + BigRoomAddZ;
					}
					int tempInt;
					tempInt = tempGridStruct->maxX;
					tempGridStruct->maxX = tempGridStruct->maxY;
					tempGridStruct->maxY = tempInt;
				}

			}
			appendedGridStruct.push_back(tempGridStruct);

#pragma endregion

		

#pragma region PROPS


			if (!randomizer.m_rooms[index].propsPlaced)
			{
				ImporterLibrary::PropItemToEngine tempProps = loader.readPropsFile(MODNAMESTRING); //TODO MAKE RANDOM 
				for (int k = 0; k < tempProps.nrOfItems; k++)
				{
					asset = DBG_NEW BaseActor();

					if (isRotated == true)
					{
						tempProps.props[k].transform_rotation[1] += 90.f;
						//tempProps.props[k].transform_rotation[1] += 90.f; // TODO WHEN DOUBLE
						//tempProps.props[k].transform_rotation[2] += 90.f;
					}
					asset->Init(*m_worldPtr, e_staticBody, tempProps.props[k].BBOX_INFO[0], tempProps.props[k].BBOX_INFO[1], tempProps.props[k].BBOX_INFO[2]);

					asset->setPosition(j + tempProps.props[k].transform_position[0] + BigRoomAddX, tempProps.props[k].transform_position[1], i + tempProps.props[k].transform_position[2] + BigRoomAddZ, true);
					if(isRotated)
						asset->setPosition(j + tempProps.props[k].transform_position[2] , tempProps.props[k].transform_position[1], i + 10 + tempProps.props[k].transform_position[0], true);

					asset->setScale(tempProps.props[k].transform_scale[0], tempProps.props[k].transform_scale[1], tempProps.props[k].transform_scale[2]);
					asset->setRotation(tempProps.props[k].transform_rotation[0], tempProps.props[k].transform_rotation[1], tempProps.props[k].transform_rotation[2]);
					switch (tempProps.props[k].typeOfProp)
					{
					case(8):
						Manager::g_meshManager.loadStaticMesh("BANNER");
						Manager::g_textureManager.loadTextures("BANNER");
						asset->setModel(Manager::g_meshManager.getStaticMesh("BANNER"));
						asset->setTexture(Manager::g_textureManager.getTexture("BANNER"));
						break;
					case(7):
						Manager::g_meshManager.loadStaticMesh("BARREL");
						Manager::g_textureManager.loadTextures("BARREL");
						asset->setModel(Manager::g_meshManager.getStaticMesh("BARREL"));
						asset->setTexture(Manager::g_textureManager.getTexture("BARREL"));
						break;
					default:
						break;
					}
					m_generated_assetVector.push_back(asset);
				}
				delete tempProps.props;
				randomizer.m_rooms[index].propsPlaced = true;
				if (randomizer.m_rooms[index].type == 0 || randomizer.m_rooms[index].type == 1)
					randomizer.m_rooms[randomizer.m_rooms[index].pairedWith].propsPlaced = true;
			}
		
#pragma endregion		

		
			widthCounter++;
		}
		depthCounter++;

	}

	ImporterLibrary::GridStruct * gridStructToSendBack = DBG_NEW ImporterLibrary::GridStruct();
	int gridTotalCount = 0;
	int widthCount = 0;
	int depthCount = 0;
	/*for (int i = 0; i < m_roomGridDepth; i++)
		depthCount += appendedGridStruct[i]->maxY;
	for (int i = 0; i < m_roomGridWidth; i++)
		widthCount += appendedGridStruct[i]->maxX;*/
	widthCount = 105;
	depthCount = 105;
	for (int i = 0; i < appendedGridStruct.size(); i++)
		gridTotalCount += appendedGridStruct[i]->nrOf;

	gridStructToSendBack->gridPoints = DBG_NEW ImporterLibrary::GridPointStruct[gridTotalCount];
	gridStructToSendBack->maxX = widthCount;
	gridStructToSendBack->maxY = depthCount;
	gridStructToSendBack->nrOf = gridTotalCount;

	int indexInGridStruct = 0;
	for (int i = 0; i < appendedGridStruct.size(); i++)
		for (int j = 0; j < appendedGridStruct[i]->nrOf; j++)
			gridStructToSendBack->gridPoints[indexInGridStruct++] = appendedGridStruct[i]->gridPoints[j];
	m_generatedGrid->CreateGridWithWorldPosValues(*gridStructToSendBack);
	returnableRoom->setGrid(m_generatedGrid);
	
	for (int i = 0; i < appendedGridStruct.size(); i++)
	{
		delete [] appendedGridStruct[i]->gridPoints;
		delete appendedGridStruct[i];
	}

	delete [] gridStructToSendBack->gridPoints;
	delete gridStructToSendBack;
}


//void RoomGenerator::_placeProps()
//{
//
//	int toBeARandomNumberInTheFuture = 1;
//
//	ImporterLibrary::CustomFileLoader loader;
//
//
//	Manager::g_meshManager.loadStaticMesh("MOD2");
//	Manager::g_meshManager.loadStaticMesh("MOD3");
//	returnableRoom->CollisionBoxes = DBG_NEW BaseActor();
//	ImporterLibrary::CollisionBoxes  modCollisionBoxes;
//	modCollisionBoxes.boxes = nullptr;
//	ImporterLibrary::PointLights lights = loader.readLightFile("MOD2");
//	for (int i = 0; i < lights.nrOf; i++)
//	{
//		_generateLights(lights.lights[i].translate[0], lights.lights[i].translate[1], lights.lights[i].translate[2], lights.lights[i].color[0],
//			lights.lights[i].color[1], lights.lights[i].color[2], lights.lights[i].intensity);
//	}
//
//	using namespace DirectX;
//
//	///////////////////////////// HÄR MELLAN
//	int iskip = 0, jskip = 0;
//	int is = rand() % (m_roomGridWidth - 1), js = rand() % (m_roomGridDepth - 1);
//	is = 1;
//	js = 1;
//
//	int ix = -1;
//	int r = rand() % 2;
//
//	//L�CKER MINNE MELLAN
//	for (float i = -m_roomWidth + 10.f; i <= m_roomWidth - 10.f; i += 20.f)
//	{
//		int jy = -1;
//		ix++;
//		for (float j = -m_roomDepth + 10.f; j <= m_roomDepth - 10.f; j += 20.f)
//		{
//			jy++;
//			r = 2;
//			if (ix == is && jy == js)
//			{
//				r = 3;
//		
//				iskip = 0;
//				jskip = 1;
//				if (rand() % 2)
//				{
//					iskip = 1;
//					jskip = 0;
//				}
//		
//			}
//			if (ix == is + iskip && jy == js + jskip)
//			{
//				iskip = 0;
//				jskip = 0;
//				is = rand() % (ix - m_roomGridWidth - 1);
//				js = rand() % (jy - m_roomGridDepth - 1);
//				r = rand() % 2;
//				std::cout << "I AM RANDOM" << std::endl;
//				continue;
//			}
//
//
//			modCollisionBoxes = loader.readMeshCollisionBoxes("../Assets/MOD" + std::to_string(r) + "FOLDER/MOD" + std::to_string(r) + "_BBOX.bin");
//
//			asset = DBG_NEW BaseActor();
//
//			if (r == 3)
//			{
//				if (iskip == 1)
//				{
//					asset->setRotation(0, 90, 0, false);
//					asset->setScale(1, 1, 1);
//				}
//			}
//			if (r == 2)
//				asset->setScale(1, 1, 1);
//
//
//			XMMATRIX roomSpace = DirectX::XMLoadFloat4x4A(&asset->getWorldmatrix());
//			roomSpace = DirectX::XMMatrixTranspose(roomSpace);
//
//			//COLLISION BOXES
//			XMVECTOR decomposeTranslation;
//			XMVECTOR decomposeRotation;
//			XMVECTOR decomposeScaling;
//			for (int k = 0; k < modCollisionBoxes.nrOfBoxes && true; k++)
//			{
//				XMFLOAT3 cPos = XMFLOAT3(modCollisionBoxes.boxes[k].translation);
//				XMFLOAT3 cScl = XMFLOAT3(modCollisionBoxes.boxes[k].scale);
//				XMFLOAT4 cQuat = XMFLOAT4(modCollisionBoxes.boxes[k].rotation);
//
//				DirectX::XMMATRIX boxTranslationMatrix = DirectX::XMMatrixTranslation(cPos.x, cPos.y, cPos.z);
//				DirectX::XMMATRIX boxRotationMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&cQuat));
//				DirectX::XMMATRIX boxScaleMatrix = DirectX::XMMatrixScaling(cScl.x, cScl.y, cScl.z);
//
//				DirectX::XMMATRIX itemWorldSpace = boxScaleMatrix * boxRotationMatrix * boxTranslationMatrix;
//
//
//				itemWorldSpace = itemWorldSpace * roomSpace;
//				
//
//				XMMatrixDecompose(&decomposeScaling, &decomposeRotation, &decomposeTranslation, itemWorldSpace);
//
//				modCollisionBoxes.boxes[k].translation[0] = XMVectorGetX(decomposeTranslation);
//				modCollisionBoxes.boxes[k].translation[1] = XMVectorGetY(decomposeTranslation);
//				modCollisionBoxes.boxes[k].translation[2] = XMVectorGetZ(decomposeTranslation);
//
//				modCollisionBoxes.boxes[k].rotation[0] = XMVectorGetX(decomposeRotation);
//				modCollisionBoxes.boxes[k].rotation[1] = XMVectorGetY(decomposeRotation);
//				modCollisionBoxes.boxes[k].rotation[2] = XMVectorGetZ(decomposeRotation);
//				modCollisionBoxes.boxes[k].rotation[3] = XMVectorGetW(decomposeRotation);
//
//				modCollisionBoxes.boxes[k].scale[0] = XMVectorGetX(decomposeScaling);
//				modCollisionBoxes.boxes[k].scale[1] = XMVectorGetY(decomposeScaling);
//				modCollisionBoxes.boxes[k].scale[2] = XMVectorGetZ(decomposeScaling);
//
//			}
//
//
//				asset->Init(*m_worldPtr, modCollisionBoxes);
//
//			if (r == 3)
//			{
//				asset->setPosition(i + (10.f * iskip), 2.5, j + (10.f * jskip), true);
//			//	asset->p_createBoundingBox(DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(DirectX::XMVectorGetX(decomposeScaling) * 2, DirectX::XMVectorGetY(decomposeScaling) * 2, DirectX::XMVectorGetZ(decomposeScaling) * 2));
//
//			}
//			else
//			{
//				asset->setPosition(i, 2.5, j);
//			//	asset->p_createBoundingBox(DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(DirectX::XMVectorGetX(decomposeScaling)*2, DirectX::XMVectorGetY(decomposeScaling) * 2, DirectX::XMVectorGetZ(decomposeScaling) * 2));
//			}
//
//				asset->setModel(Manager::g_meshManager.getStaticMesh("MOD" + std::to_string(r)));
//				asset->setTexture(Manager::g_textureManager.getTexture("WALL"));
//				asset->setTextureTileMult(5, 5);
//				m_generated_assetVector.push_back(asset);
//
//
//
//			//PROPS
//			//ImporterLibrary::PropItemToEngine tempProps = loader.readPropsFile("MOD" + std::to_string(r));
//			//for (int k = 0; k < tempProps.nrOfItems; k++)
//			//{
//			//	
//			//	XMFLOAT3 propPos = XMFLOAT3(tempProps.props[k].transform_position);
//			//	XMFLOAT3 propScale = XMFLOAT3(tempProps.props[k].transform_scale);
//			//	//XMFLOAT4 propRotation = XMFLOAT4(tempProps.props[k].transform_rotation);
//
//			//	DirectX::XMMATRIX propTranslationMatrix = DirectX::XMMatrixTranslation(propPos.x, propPos.y, propPos.z);
//			//	DirectX::XMMATRIX propRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(tempProps.props[k].transform_rotation[0], tempProps.props[k].transform_rotation[1], tempProps.props[k].transform_rotation[2]);
//			//	DirectX::XMMATRIX propScaleMatrix = DirectX::XMMatrixScaling(propScale.x, propScale.y, propScale.z);
//
//			//	DirectX::XMMATRIX itemWorldSpace = propScaleMatrix * propRotationMatrix * propTranslationMatrix;
//
//
//			//	itemWorldSpace = itemWorldSpace * roomSpace;
//			//	XMVECTOR decomposeTranslation;
//			//	XMVECTOR decomposeRotation;
//			//	XMVECTOR decomposeScaling;
//
//			//	XMMatrixDecompose(&decomposeScaling, &decomposeRotation, &decomposeTranslation, itemWorldSpace);
//
//			//	if (iskip == 1)
//			//	{
//			//		tempProps.props[k].transform_rotation[0] += 90.f;
//			//		tempProps.props[k].transform_rotation[1] += 90.f;
//			//		tempProps.props[k].transform_rotation[2] += 90.f;
//			//	}
//			//	asset = DBG_NEW BaseActor();
//			//	asset->Init(*m_worldPtr, e_staticBody, tempProps.props[k].BBOX_INFO[0], tempProps.props[k].BBOX_INFO[1], tempProps.props[k].BBOX_INFO[2]);
//
//			//	if (r == 3)
//			//	{
//			//		asset->setPosition(i + (7.5f * iskip) + tempProps.props[k].transform_position[0], 2.5 +tempProps.props[k].transform_position[1], j + (10.0f * jskip) + tempProps.props[k].transform_position[2], true);
//			//		asset->setScale(tempProps.props[k].transform_scale[0], tempProps.props[k].transform_scale[1], tempProps.props[k].transform_scale[2]);
//			//		asset->p_createBoundingBox(DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(tempProps.props[k].BBOX_INFO[0], tempProps.props[k].BBOX_INFO[1], tempProps.props[k].BBOX_INFO[2]));
//
//			//	}
//			//	else
//			//	{
//			//		asset->setPosition(i + tempProps.props[k].transform_position[0], 2.5 + tempProps.props[k].transform_position[1], j + tempProps.props[k].transform_position[2], true);
//			//		asset->setScale(tempProps.props[k].transform_scale[0], tempProps.props[k].transform_scale[1], tempProps.props[k].transform_scale[2]);
//			//		asset->p_createBoundingBox(DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(tempProps.props[k].BBOX_INFO[0], tempProps.props[k].BBOX_INFO[1], tempProps.props[k].BBOX_INFO[2]));
//			//	}
//			//	switch (tempProps.props[k].typeOfProp)
//			//	{
//			//	case(8):
//			//		Manager::g_meshManager.loadStaticMesh("BANNER");
//			//		Manager::g_textureManager.loadTextures("BANNER");
//			//		asset->setModel(Manager::g_meshManager.getStaticMesh("BANNER"));
//			//		asset->setTexture(Manager::g_textureManager.getTexture("BANNER"));
//			//		break;
//			//	case(7):
//			//		Manager::g_meshManager.loadStaticMesh("BARREL");
//			//		Manager::g_textureManager.loadTextures("BARREL");
//			//		asset->setModel(Manager::g_meshManager.getStaticMesh("BARREL"));
//			//		asset->setTexture(Manager::g_textureManager.getTexture("BARREL"));
//			//		break;
//			//	default:
//			//		break;
//			//	}
//			//	m_generated_assetVector.push_back(asset);
//			//}
//			//delete tempProps.props;
//			if (modCollisionBoxes.boxes)
//				delete[] modCollisionBoxes.boxes;
//			if (lights.lights)
//				delete[] lights.lights;
//		}
//	}
//}


void RoomGenerator::_createEnemies()
{
	Enemy * enemy;
	
	Manager::g_meshManager.loadDynamicMesh("STATE");
	Manager::g_textureManager.loadTextures("SPHERE");
	for (int i = 0; i < m_nrOfEnemies; i++)
	{
		int x = returnRandomInGridWidth();
		int z = returnRandomInGridDepth();
		enemy = DBG_NEW Enemy(m_worldPtr, x, 15 , z);
		DirectX::XMFLOAT4A pos = enemy->getPosition();
		Tile finPos = Tile(int(returnRandomInGridWidth()), int(returnRandomInGridDepth()));
		Tile enemyPos = m_generatedGrid->WorldPosToTile(pos.x, pos.z);
		enemy->SetPathVector(this->m_generatedGrid->FindPath(enemyPos, finPos));
		m_generatedRoomEnemies.push_back(enemy);
	}

}

void RoomGenerator::_FindWinnableAndGuardPaths()
{
	int playerStartPos = returnRandomInGridWidth();
	int playerWinsAt = returnRandomInGridWidth();

	//returnableRoom->setPlayer1StartPos(DirectX::XMFLOAT4(playerStartPos, 6, 1 - m_roomDepth, 1));
	//returnableRoom->setPlayer2StartPos(DirectX::XMFLOAT4(playerStartPos + 2, 6, 1 - m_roomDepth, 1));


	Tile currentTile;
	Tile pathToTile;
	std::vector<Node*> nodes;
	currentTile = m_generatedGrid->WorldPosToTile(playerStartPos, 1 - m_roomDepth);
	for (int i = 0; i < 3; i++)
	{
		for (int y = 0; y < 3; y++)
		{
			if (y == 1 && i == 6)
				pathToTile = m_generatedGrid->WorldPosToTile(playerWinsAt, m_roomDepth);
			else
				pathToTile = m_generatedGrid->WorldPosToTile(returnRandomInGridWidth(), returnRandomInGridDepth());

			std::vector<Node*> tempPath = m_generatedGrid->FindPath(currentTile, pathToTile);

			nodes.insert(std::end(nodes), std::begin(tempPath), std::end(tempPath));
			currentTile = pathToTile;
		}
	}
	currentTile = m_generatedGrid->WorldPosToTile(playerStartPos, 1 - m_roomDepth);
	for (int i = 0; i < 3; i++)
	{
		for (int y = 0; y < 3; y++)
		{

			if (y == 1 && i == 6)
				pathToTile = m_generatedGrid->WorldPosToTile(playerWinsAt, m_roomDepth);
			else
				pathToTile = m_generatedGrid->WorldPosToTile(returnRandomInGridWidth(), returnRandomInGridDepth());

			std::vector<Node*> tempPath = m_generatedGrid->FindPath(currentTile, pathToTile);

			nodes.insert(std::end(nodes), std::begin(tempPath), std::end(tempPath));
			currentTile = pathToTile;

		}
	}
	for (int i = 0; i < nodes.size(); i++)
		nodes[i]->tile.setPathable(false);


	for (int i = 0; i < nodes.size(); i++)
	{
		nodes[i]->tile.setPathable(false);
		delete nodes.at(i);
	}
}

void RoomGenerator::_generateLights(float xPos, float yPos, float zPos, float colorR, float colorG, float colorB, float intensity)
{
	float lightPos[3];
	lightPos[0] = xPos;
	lightPos[1] = yPos;
	lightPos[2] = zPos;
	float color[3];
	color[0] = colorR;
	color[1] = colorG;
	color[2] = colorB;
	std::cout << lightPos[0] << " " << lightPos[1] << lightPos[2] << std::endl;
	PointLight * tempLight = DBG_NEW PointLight(lightPos, color, intensity);
	m_generated_pointLightVector.push_back(tempLight);
}

int RoomGenerator::returnRandomInGridWidth()
{
	float randomNr = (float)rand() / RAND_MAX;
	float min = 1 - m_roomWidth;
	float max = m_roomWidth - 1;
	//return (rand() % (int)m_roomWidth +1) - m_roomWidth;
	return (min + randomNr * (max - (min)));
}

int RoomGenerator::returnRandomInGridDepth()
{
	float randomNr = (float)rand() / RAND_MAX;
	float min = 1 - m_roomDepth;
	float max = m_roomDepth - 1;
	//return (rand() % (int)m_roomDepth +1) - m_roomDepth;
	return (min + randomNr * (max - (min)));
}

int RoomGenerator::returnRandomBetween(int min, int max)
{
	float randomNr = (float)rand() / RAND_MAX;
	return (min + randomNr * (max - (min)));
}



Room * RoomGenerator::getGeneratedRoom( b3World * worldPtr, int arrayIndex, Player *  playerPtr)
{
	
	this->m_worldPtr = worldPtr;
	Manager::g_meshManager.loadStaticMesh("FLOOR");
	Manager::g_textureManager.loadTextures("FLOOR");
	Manager::g_meshManager.loadStaticMesh("WALL");
	Manager::g_textureManager.loadTextures("WALL");

	



	returnableRoom = DBG_NEW Room(worldPtr, arrayIndex, playerPtr);
	returnableRoom->setPlayer1StartPos(DirectX::XMFLOAT4(0, 10, 0, 1));
	returnableRoom->setPlayer2StartPos(DirectX::XMFLOAT4(0, 10, 0, 1));
	//std::vector<Enemy*> enemies, Player * player, Grid * grid
	_generateGrid();
	//_FindWinnableAndGuardPaths();
	returnableRoom->setGrid(this->m_generatedGrid);
	//_placeProps();
	

	

	_makeWalls();
	_makeFloor();
	//_createEnemies();
	
	for (int i = 0; i < 12; i++)
	{
		_generateLights(i*5, 4, i*5, 50, 60, 70, 10);	
	}
	m_generatedRoomEnemyHandler = DBG_NEW EnemyHandler;
	m_generatedRoomEnemyHandler->Init(m_generatedRoomEnemies, playerPtr, this->m_generatedGrid);

	dbgFuncSpawnAboveMap();

	returnableRoom->setEnemyhandler(m_generatedRoomEnemyHandler);
	returnableRoom->setStaticMeshes(m_generated_assetVector);
	returnableRoom->setLightvector(m_generated_pointLightVector);
	returnableRoom->setRoomGuards(m_generatedRoomEnemies);
	return returnableRoom;
}
