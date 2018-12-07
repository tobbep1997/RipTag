#include "RipTagPCH.h"
#include "Grid.h"

Grid::Grid(int width, int height)
{
	m_width = width;
	m_height = height;
	
	for (int i = 0; i < m_height; i++)
		for (int j = 0; j < m_width; j++)
			m_nodeMap.push_back(Tile(j, i));
}

Grid::Grid(float xVal, float yVal, int width, int depth)
{
	m_width = width;
	m_height = depth;
	float tempXval = xVal;
	for (int i = 0; i < depth; i++)
	{
		xVal = tempXval;
		for (size_t j = 0; j < width; j++)
		{
			m_nodeMap.push_back(Node(Tile(j, i),
				NodeWorldPos(xVal,
					yVal)));
			xVal += 1;
		}
		yVal += 1;
	}
}

Grid::~Grid()
{
}

Tile Grid::WorldPosToTile(float x, float y)
{
	int approximateWorldPosX = (int)x;
	int approximateWorldPosY = (int)y;
	int index = _worldPosInNodeMap(0, m_height * m_width - 1, approximateWorldPosX, approximateWorldPosY);
	if (index == -1)
		return Tile();
	
	return m_nodeMap[index].tile;
}

Node Grid::GetWorldPosFromIndex(int index)
{
	return m_nodeMap.at(index);
}

void Grid::BlockGridTile(int index, bool pathable)
{
	m_nodeMap.at(index).tile.setPathable(pathable);
}

void Grid::CreateGridWithWorldPosValues(ImporterLibrary::GridStruct grid)
{
	if (grid.gridPoints == nullptr)
		return;
	if (!m_nodeMap.empty())
		m_nodeMap.clear();
	
	m_width = grid.maxX;
	m_height = grid.maxY;
	for (int i = 0; i < m_height; i++)
		for (int j = 0; j < m_width; j++)
		{
			int index = i + j * m_height;
			m_nodeMap.push_back(Node(Tile(j, i, grid.gridPoints[index].pathable),
				NodeWorldPos(grid.gridPoints[index].translation[0],
					grid.gridPoints[index].translation[2])));
		}
	_createSubGrid();
	/*std::ofstream o;
	this->PrintMe();
	o.open("sub.txt");
	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			if (m_nodeMap[x + y * m_width].tile.getSubGrid() == -1)
				o << "#";
			else
				o << m_nodeMap[x + y * m_width].tile.getSubGrid();
			o << " ";
		}
		o << "\n";
	}
	o.close();*/
}

void Grid::CreateGridFromRandomRoomLayout(ImporterLibrary::GridStruct grid)
{
	if (grid.gridPoints == nullptr)
		return;
	if (!m_nodeMap.empty())
		m_nodeMap.clear();

	m_width = grid.maxX;
	m_height = grid.maxY;
	for (int i = 0; i < grid.maxY; i++)
		for (int j = 0; j < grid.maxX; j++)
		{
			int index = j + i * grid.maxX;
			m_nodeMap.push_back(Node(Tile(j, i, grid.gridPoints[index].pathable),
				NodeWorldPos(grid.gridPoints[index].translation[0],
					grid.gridPoints[index].translation[2])));
		}
}

void Grid::GenerateRoomNodeMap(RandomRoomGrid * randomizer)
{
	int width = randomizer->GetSize().x * 2 - 1;
	int depth = randomizer->GetSize().y * 2 - 1;
	float worldX = -40.f;
	float worldY = -40.f;

	for (int i = 0; i < depth; i++)
	{
		for (int j = 0; j < width; j++)
		{
			bool pathable = false;
			if (i % 2 == 0 && j % 2 == 0)
				pathable = true;
			m_roomNodeMap.push_back(Node(Tile(j, i, pathable), NodeWorldPos(worldX + j * 10.0f, worldY + i * 10.0f)));
		}
	}
	int counter = 0;
	for (int i = 0; i < depth; i += 2)
	{
		for (int j = 0; j < width; j += 2)
		{
			int index = counter++;
			/*if (randomizer->m_rooms[index].type == 1)
			{
				if (j < width - 1)
					m_roomNodeMap[(j + 1) + i * width].tile.setPathable(randomizer->m_rooms[index].south);
				if (i < depth - 1)
					m_roomNodeMap[j + (i + 1) * width].tile.setPathable(randomizer->m_rooms[index].west);
			}*/
			
			if (j < width - 1)
				m_roomNodeMap[(j + 1) + i * width].tile.setPathable(randomizer->m_rooms[index].east);
			if (i < depth - 1)
				m_roomNodeMap[j + (i + 1) * width].tile.setPathable(randomizer->m_rooms[index].south);
		}
	}
}

std::vector<Node*> Grid::FindPath(Tile source, Tile destination)
{
	if (source.getX() < 0 || source.getY() < 0)
	{
		std::vector<Node*> temp;
		
		return temp;
	}
	if (source.getX() < 0 || source.getY() < 0)
	{
		std::vector<Node*> temp;

		return temp;
	}

	Tile dest = m_nodeMap.at(destination.getX() + destination.getY() * m_width).tile;
	Tile src = m_nodeMap.at(source.getX() + source.getY() * m_width).tile;
	if (!src.getPathable())
		source = GetRandomNearbyUnblockedTile(source);
	if (!dest.getPathable())
		destination = GetRandomNearbyUnblockedTile(destination);
	if (!m_roomNodeMap.empty() && !_tilesAreInTheSameRoom(source, destination))
	{		
		
		// A* through the "large" grid to find which rooms are connected in the path
		std::vector<Node*> roomNodePath = _findRoomNodePath(source, destination);

		_removeAllBlockedTiles(roomNodePath);

		if (roomNodePath.empty())
			return _findPath(source, destination, m_nodeMap, m_width, m_height);

		// A* in each room to get to the next
		std::vector<TilePair> tilePairs = _roomNodePathToGridTiles(&roomNodePath, source, destination);
		
		/*static int counter = 0;
		std::ofstream file;
		file.open("PATH_" + std::to_string(counter++) + ".txt");

		_printTilePairs(tilePairs, file, source, destination);*/

		std::vector<Node*> pathToDestination;
		for (auto & tp : tilePairs)
		{
			std::vector<Node*> partOfPath = _findPath(tp.source, tp.destination, m_nodeMap, m_width, m_height);

			pathToDestination.insert(std::end(pathToDestination), std::begin(partOfPath), std::end(partOfPath));
			//_printPath(partOfPath, file, source, destination);
		}

		//_printPath(pathToDestination, file, source, destination);

		//file.close();

		for (int i = 0; i < roomNodePath.size(); i++)
			delete roomNodePath[i];
		roomNodePath.clear();

		return pathToDestination;
	}
	else
	{
		if (dest.getSubGrid() == src.getSubGrid())
			return _findPath(source, destination, m_nodeMap, m_width, m_height);
		else
			return std::vector<Node*>();
	}
}

Tile Grid::GetRandomNearbyUnblockedTile(Tile src)
{
	bool unblocked = false;
	int x = src.getX();
	int y = src.getY();
	int count = 0;
	bool blockedDirections[8];

	// North -> East -> South -> West
	// Northeast -> Northwest -> Southeast -> Southwest
	for (int i = 0; i < 8; i++)
		blockedDirections[i] = false;

	Tile returnTile;
	int tempX = 0;
	int tempY = 0;
	while (!unblocked)
	{
		// North (0, -1)
		if (!blockedDirections[0])
		{
			tempY = y - 1 * count;
			if (tempY < 0)
				blockedDirections[0] = true;
			else
			{
				tempY = max(tempY, 0);
				if (!_tilesAreInTheSameRoom(Tile(x, y), Tile(x, tempY)))
					blockedDirections[0] = true;
				else if (m_nodeMap[x + tempY * m_width].tile.getPathable())
				{
					returnTile = m_nodeMap[x + tempY * m_width].tile;
					unblocked = true;
				}
			}
		}
		// East (1, 0)
		if (!blockedDirections[1])
		{
			tempX = x + 1 * count;
			if (tempX >= m_width)
				blockedDirections[1] = true;
			else
			{
				tempX = min(tempX, m_width - 1);
				if (!_tilesAreInTheSameRoom(Tile(x, y), Tile(tempX, y)))
					blockedDirections[1] = true;
				else if (m_nodeMap[tempX + y * m_width].tile.getPathable())
				{
					returnTile = m_nodeMap[tempX + y * m_width].tile;
					unblocked = true;
				}
			}
		}
		// South (0, 1)
		if (!blockedDirections[2])
		{
			tempY = y + 1 * count;
			if (tempY >= m_height)
				blockedDirections[2] = true;
			else
			{
				tempY = min(tempY, m_height - 1);
				if (!_tilesAreInTheSameRoom(Tile(x, y), Tile(x, tempY)))
					blockedDirections[2] = true;
				else if (m_nodeMap[x + tempY * m_width].tile.getPathable())
				{
					returnTile = m_nodeMap[x + tempY * m_width].tile;
					unblocked = true;
				}
			}
		}
		// West (-1, 0)
		if (!blockedDirections[3])
		{
			tempX = x - 1 * count;
			if (tempX < 0)
				blockedDirections[3] = true;
			else
			{
				tempX = max(tempX, 0);
				if (!_tilesAreInTheSameRoom(Tile(x, y), Tile(tempX, y)))
					blockedDirections[3] = true;
				else if (m_nodeMap[tempX + y * m_width].tile.getPathable())
				{
					returnTile = m_nodeMap[tempX + y * m_width].tile;
					unblocked = true;
				}
			}
		}
		// Northeast (1, -1)
		if (!blockedDirections[4])
		{
			tempY = y - 1 * count;
			if (tempY < 0)
				blockedDirections[4] = true;
			else
			{
				tempY = max(tempY, 0);
				tempX = x + 1 * count;
				if (tempX >= m_width)
					blockedDirections[4] = true;
				else
				{
					tempX = min(tempX, m_width - 1);
					if (!_tilesAreInTheSameRoom(Tile(x, y), Tile(tempX, tempY)))
						blockedDirections[4] = true;
					else if (m_nodeMap[tempX + tempY * m_width].tile.getPathable())
					{
						returnTile = m_nodeMap[tempX + tempY * m_width].tile;
						unblocked = true;
					}
				}
			}
		}
		// Northwest (-1, -1)
		if (!blockedDirections[5])
		{
			tempY = y - 1 * count;
			if (tempY < 0)
				blockedDirections[5] = true;
			else
			{
				tempY = max(tempY, 0);
				tempX = x - 1 * count;
				if (tempX < 0)
					blockedDirections[5] = true;
				else
				{
					tempX = max(tempX, 0);
					if (!_tilesAreInTheSameRoom(Tile(x, y), Tile(tempX, tempY)))
						blockedDirections[5] = true;
					else if (m_nodeMap[tempX + tempY * m_width].tile.getPathable())
					{
						returnTile = m_nodeMap[tempX + tempY * m_width].tile;
						unblocked = true;
					}
				}
			}
		}
		// Southeast (1, 1)
		if (!blockedDirections[6])
		{
			tempY = y + 1 * count;
			if (tempY >= m_height)
				blockedDirections[6];
			else
			{
				tempY = min(tempY, m_height - 1);
				tempX = x + 1 * count;
				if (tempX >= m_width)
					blockedDirections[6];
				else
				{
					tempX = min(tempX, m_width - 1);
					if (!_tilesAreInTheSameRoom(Tile(x, y), Tile(tempX, tempY)))
						blockedDirections[6] = true;
					else if (m_nodeMap[tempX + tempY * m_width].tile.getPathable())
					{
						returnTile = m_nodeMap[tempX + tempY * m_width].tile;
						unblocked = true;
					}
				}
			}
		}
		// Southwest (-1, 1)
		if (!blockedDirections[7])
		{
			tempY = y + 1 * count;
			if (tempY >= m_height)
				blockedDirections[7] = true;
			else
			{
				tempY = min(tempY, m_height - 1);
				tempX = x - 1 * count;
				if (tempX < 0)
					blockedDirections[7];
				else
				{
					tempX = max(tempX, 0);
					if (!_tilesAreInTheSameRoom(Tile(x, y), Tile(tempX, tempY)))
						blockedDirections[7] = true;
					else if (m_nodeMap[tempX + tempY * m_width].tile.getPathable())
					{
						returnTile = m_nodeMap[tempX + tempY * m_width].tile;
						unblocked = true;
					}
				}
			}
		}
		if (!unblocked)
			unblocked = _isAllDirectionsBlocked(blockedDirections);
		count++;
	}
	return returnTile;
}

int Grid::getGridWidth()
{
	return m_width;
}

int Grid::getGridHeight()
{
	return m_height;
}

Node* Grid::GetNodeAt(int index)
{
	return &m_nodeMap.at(index);
}

void Grid::BlockIfNotPathable(int targetX, int targetY)
{
	if (m_nodeMap.at(targetX + targetY * m_width).tile.getPathable())
	{
		std::vector<Node*> targets;
		_blockCheck(targetX, targetY, targets);
		if (targets.size() < MAX_BLOCK_CHECK)
			for (auto & n : targets)
				n->tile.setPathable(false);
	}
}

void Grid::PrintMe() const
{
	std::ofstream map;
	map.open("Full_map.txt");
	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			int index = x + y * m_width;
			if (m_nodeMap[index].tile.getPathable())
				map << " ";
			else
				map << "#";
			map << " ";
		}
		map << "\n";
	}
	map.close();
}

void Grid::_checkNode(std::shared_ptr<Node> current, float addedGCost, int offsetX, int offsetY,
						Tile dest, std::vector<std::shared_ptr<Node>> & openList,
						std::vector<Node> & nodeMap, bool * closedList, int width, int height)
{
	int currentX = current->tile.getX();
	int currentY = current->tile.getY();
	Tile nextTile = Tile(currentX + offsetX, currentY + offsetY);
	int nextTileIndex = nextTile.getX() + nextTile.getY() * width;

	if (_isValid(nextTile, width, height) && !closedList[nextTileIndex] &&
		nodeMap.at(nextTileIndex).tile.getPathable())
	{
		std::shared_ptr<Node> newNode = std::make_shared<Node>(nodeMap.at(nextTileIndex).tile, nodeMap.at(nextTileIndex).worldPos,
			current, current->gCost + addedGCost, _calcHValue(nextTile, dest));
		openList.push_back(newNode);
	}
}

const bool Grid::_isValid(Tile tile, int width, int height) const
{
	int x = tile.getX();
	int y = tile.getY();
	return (x >= 0) && (x < width) &&
		(y >= 0) && (y < height);
}

const float Grid::_calcHValue(Tile src, Tile dest) const
{
	int x = abs(src.getX() - dest.getX());
	int y = abs(src.getY() - dest.getY());
	//return (x + y) + (-0.414f) * min(x, y);
	return 1.0f * (x + y) + (1.414f - 2 * 1.0f) * min(x, y);
}

int Grid::_worldPosInNodeMap(int begin, int end, int x, int y) const
{
	if (begin <= end)
	{
		int mid = begin + (end - begin) / 2;
		if ((int)m_nodeMap.at(mid).worldPos.y == y)
		{
			int indexAdjuster = mid % m_width;
			return _findXInYRow(mid - indexAdjuster, mid - indexAdjuster + m_width - 1, x, y);
		}

		if (y < m_nodeMap.at(mid).worldPos.y)
			return _worldPosInNodeMap(begin, mid - 1, x, y);

		return _worldPosInNodeMap(mid + 1, end, x, y);
	}
	return -1;
}

int Grid::_findXInYRow(int begin, int end, int x, int y) const
{
	if (begin <= end)
	{
		int mid = begin + (end - begin) / 2;

		if ((int)m_nodeMap.at(mid).worldPos.x == x)
			return mid;

		if (x < m_nodeMap.at(mid).worldPos.x)
			return _findXInYRow(begin, mid - 1, x, y);

		return _findXInYRow(mid + 1, end, x, y);
	}
	return -1;
}

const bool Grid::_tilesAreInTheSameRoom(const Tile & source, const Tile & destination)
{
	NodeWorldPos sWpos = m_nodeMap[source.getX() + source.getY() * m_width].worldPos;
	NodeWorldPos dWpos = m_nodeMap[destination.getX() + destination.getY() * m_width].worldPos;
	DirectX::XMFLOAT2 sWposOff = { sWpos.x + 50.0f, sWpos.y + 50.0f };
	DirectX::XMFLOAT2 dWposOff = { dWpos.x + 50.0f, dWpos.y + 50.0f };
	DirectX::XMINT2 sIndex2D = { (int)sWposOff.x / 20, (int)sWposOff.y / 20 };
	DirectX::XMINT2 dIndex2D = { (int)dWposOff.x / 20, (int)dWposOff.y / 20 };
	const int ROOM_WIDTH = 5;

	sIndex2D.x = std::clamp(sIndex2D.x, 0, 4);
	sIndex2D.y = std::clamp(sIndex2D.y, 0, 4);
	dIndex2D.x = std::clamp(dIndex2D.x, 0, 4);
	dIndex2D.y = std::clamp(dIndex2D.y, 0, 4);

	int sIndex = sIndex2D.y * ROOM_WIDTH + sIndex2D.x;
	int dIndex = dIndex2D.y * ROOM_WIDTH + dIndex2D.x;

	return sIndex == dIndex;
}

std::vector<Node*> Grid::_findRoomNodePath(const Tile & source, const Tile & destination)
{
	NodeWorldPos sWpos = m_nodeMap[source.getX() + source.getY() * m_width].worldPos;
	NodeWorldPos dWpos = m_nodeMap[destination.getX() + destination.getY() * m_width].worldPos;
	DirectX::XMFLOAT2 sWposOff = { sWpos.x + 40.0f, sWpos.y + 40.0f };
	DirectX::XMFLOAT2 dWposOff = { dWpos.x + 40.0f, dWpos.y + 40.0f };
	DirectX::XMINT2 sIndex2D = { (int)sWposOff.x / 10, (int)sWposOff.y / 10 };
	DirectX::XMINT2 dIndex2D = { (int)dWposOff.x / 10, (int)dWposOff.y / 10 };
	const int ROOM_WIDTH = 9;

	sIndex2D.x = std::clamp(sIndex2D.x, 0, 8);
	sIndex2D.y = std::clamp(sIndex2D.y, 0, 8);
	dIndex2D.x = std::clamp(dIndex2D.x, 0, 8);
	dIndex2D.y = std::clamp(dIndex2D.y, 0, 8);

	int sIndex = sIndex2D.y * ROOM_WIDTH + sIndex2D.x;
	int dIndex = dIndex2D.y * ROOM_WIDTH + dIndex2D.x;
	Tile roomSource = m_roomNodeMap[sIndex].tile;
	Tile roomDest = m_roomNodeMap[dIndex].tile;
	roomSource = _getCenterGridFromRoomGrid(roomSource, source);
	roomDest = _getCenterGridFromRoomGrid(roomDest, destination);

	return _findPath(roomSource, roomDest, m_roomNodeMap, 9, 9);
}

const bool Grid::_isAllDirectionsBlocked(const bool blockedDirections[8])
{
	if (blockedDirections[0] && blockedDirections[1] &&
		blockedDirections[2] && blockedDirections[3] &&
		blockedDirections[4] && blockedDirections[5] &&
		blockedDirections[6] && blockedDirections[7])
		return true;
	return false;
}

void Grid::_removeAllBlockedTiles(std::vector<Node*>& roomNodePath)
{
	int size = roomNodePath.size();
	for (int i = 0; i < size; i++)
	{
		Tile n = WorldPosToTile(roomNodePath[i]->worldPos.x, roomNodePath[i]->worldPos.y);
		if (!n.getPathable() ||
			(roomNodePath[i]->tile.getX() % 2 == 0 &&
				roomNodePath[i]->tile.getY() % 2 == 0))
		{
			delete roomNodePath.at(i);			
			roomNodePath.erase(roomNodePath.begin() + i);
			size = roomNodePath.size();
			i--;
		}
	}
}

Tile Grid::_getCenterGridFromRoomGrid(const Tile & tileOnRoomNodeMap, const Tile & tileInNodeMap)
{
	int Xroom = tileOnRoomNodeMap.getX();
	int Yroom = tileOnRoomNodeMap.getY();
	std::vector<Node*> potentialCenter;
	if (Yroom % 2 == 0 && Xroom % 2 == 0)
	{
		return tileOnRoomNodeMap;
	}
	if (Yroom % 2 == 0)
	{
		// Center is either on Left or Right
		int iLeft = Yroom * 9 + Xroom - 1;
		int iRight = Yroom * 9 + Xroom + 1;
		potentialCenter.push_back(&m_roomNodeMap.at(iLeft));
		potentialCenter.push_back(&m_roomNodeMap.at(iRight));
	}
	else if (Xroom % 2 == 0)
	{
		// Center is either on Up or Down
		int iUp = (Yroom - 1) * 9 + Xroom;
		int iDown = (Yroom + 1) * 9 + Xroom;
		potentialCenter.push_back(&m_roomNodeMap.at(iUp));
		potentialCenter.push_back(&m_roomNodeMap.at(iDown));
	}
	else
	{
		// Center is either D-Up, D-Right, D-Down, D-Left
		int leftUp = (Yroom - 1) * 9 + Xroom - 1;
		int rightUp = (Yroom - 1) * 9 + Xroom + 1;
		int rightDown = (Yroom + 1) * 9 + Xroom + 1;
		int leftDown = (Yroom + 1) * 9 + Xroom - 1;

		potentialCenter.push_back(&m_roomNodeMap.at(leftUp));
		potentialCenter.push_back(&m_roomNodeMap.at(rightUp));
		potentialCenter.push_back(&m_roomNodeMap.at(rightDown));
		potentialCenter.push_back(&m_roomNodeMap.at(leftDown));
	}
	float distance = 99999.9f;
	auto sWpos = m_nodeMap[tileInNodeMap.getX() + tileInNodeMap.getY() * 101].worldPos;
	DirectX::XMVECTOR vSWpos = DirectX::XMVectorSet(sWpos.x, sWpos.y, 0.0f, 0.0f);

	Tile center;
	for (auto & node : potentialCenter)
	{
		auto pos = node->worldPos;
		DirectX::XMVECTOR vNode = DirectX::XMVectorSet(pos.x, pos.y, 0.0f, 0.0f);

		float length = DirectX::XMVectorGetX(DirectX::XMVector2Length(DirectX::XMVectorSubtract(vNode, vSWpos)));
		if (length < distance)
		{
			center = node->tile;
			distance = length;
		}
	}

	return center;
}

std::vector<Grid::TilePair> Grid::_roomNodePathToGridTiles(std::vector<Node*> * roomNodes, const Tile & source, const Tile & destination)
{
	std::vector<Grid::TilePair> gtp;
	Grid::TilePair start;
	start.source = source;
	auto startPos = roomNodes->at(0)->worldPos;
	start.destination = WorldPosToTile(startPos.x, startPos.y);
	gtp.push_back(start);

	for (int i = 0; i < roomNodes->size() - 1; i++)
	{
		Grid::TilePair tp;
		auto wpS = roomNodes->at(i)->worldPos;
		auto wpD = roomNodes->at(i + 1)->worldPos;
				
		tp.source = WorldPosToTile(wpS.x, wpS.y);
		tp.destination = WorldPosToTile(wpD.x, wpD.y);

		gtp.push_back(tp);
	}

	Grid::TilePair end;
	auto endPos = roomNodes->at(roomNodes->size() - 1)->worldPos;
	end.source = WorldPosToTile(endPos.x, endPos.y);
	end.destination = destination;
	gtp.push_back(end);

	return gtp;
}

std::vector<Node*> Grid::_findPath(Tile source, Tile destination, std::vector<Node> & nodeMap, int width, int height)
{
	if (!_isValid(destination, width, height) || !_isValid(source, width, height))
		return std::vector<Node*>();

	Tile dest = nodeMap.at(destination.getX() + destination.getY() * width).tile;
	Tile src = nodeMap.at(source.getX() + source.getY() * width).tile;
	if (!dest.getPathable() || !src.getPathable())
		return std::vector<Node*>();

	bool * closedList = new bool[height * width];
	for (int i = 0; i < height * width; i++)
		closedList[i] = false;
	std::vector<std::shared_ptr<Node>> openList;
	std::vector<std::shared_ptr<Node>> earlyExploration;
	std::shared_ptr<Node> earlyExplorationNode = nullptr;
	std::shared_ptr<Node> current = std::make_shared<Node>(src, NodeWorldPos(), nullptr, 0.0f, _calcHValue(src, dest));
	openList.push_back(current);

	while (!openList.empty() || earlyExplorationNode != nullptr)
	{
		if (earlyExplorationNode != nullptr)
		{
			current = earlyExplorationNode;
			earlyExplorationNode = nullptr;
		}
		else
		{
			std::sort(openList.begin(), openList.end(), [](std::shared_ptr<Node> first, std::shared_ptr<Node> second) { return first->fCost < second->fCost; });
			current = openList.at(0);
			openList.erase(openList.begin());
		}

		if (current->tile == dest)
		{
			// Create complete path
			std::vector<std::shared_ptr<Node>> path;
			std::vector<Node*> returnPath;
			while (current->parent != nullptr)
			{
				std::shared_ptr<Node> nextPathNode(current);
				path.push_back(nextPathNode);
				current = nextPathNode->parent;
			}
			std::reverse(path.begin(), path.end());
			for (int i = 0; i < path.size(); i++)
				returnPath.push_back(DBG_NEW Node(path.at(i).get()));

			delete [] closedList;
			return returnPath;
		}

		// Flag the tile as visited
		closedList[current->tile.getX() + current->tile.getY() * width] = true;

		/*
			Generate all the eight successors of the cell

			  N.W	N	N.E
				\	|	/
				 \	|  /
			W --- Node --- E
				 /	|  \
				/	|	\
			  S.W	S	S.E

			Node--> Current Node	= (0, 0)
			N	--> North			= (0, -1)
			S	--> South			= (0, 1)
			W	--> West			= (-1, 0)
			E	--> East			= (1, 0)
			N.W	--> Northwest		= (-1, -1)
			N.E	--> Northeast		= (1, -1)
			S.W	-->	Southwest		= (-1, 1)
			S.E	-->	Southeast		= (1, 1)
		*/

		// AddedGCost based on the distance to the node, 1.0 for direct paths and 1.414 for diagonal paths.
		// Offset defined by the new tiles direction standing at the source tile.
		/*---------- North ----------*/
		_checkNode(current, 1.0f, 0, -1, dest, earlyExploration, nodeMap, closedList, width, height);
		/*---------- South ----------*/
		_checkNode(current, 1.0f, 0, 1, dest, earlyExploration, nodeMap, closedList, width, height);
		/*---------- West ----------*/
		_checkNode(current, 1.0f, -1, 0, dest, earlyExploration, nodeMap, closedList, width, height);
		/*---------- East ----------*/
		_checkNode(current, 1.0f, 1, 0, dest, earlyExploration, nodeMap, closedList, width, height);
		/*---------- Northwest ----------*/
		_checkNode(current, 1.414f, -1, -1, dest, earlyExploration, nodeMap, closedList, width, height);
		/*---------- Northeast ----------*/
		_checkNode(current, 1.414f, 1, -1, dest, earlyExploration, nodeMap, closedList, width, height);
		/*---------- Southwest ----------*/
		_checkNode(current, 1.414f, -1, 1, dest, earlyExploration, nodeMap, closedList, width, height);
		/*---------- Southeast ----------*/
		_checkNode(current, 1.414f, 1, 1, dest, earlyExploration, nodeMap, closedList, width, height);

		std::sort(earlyExploration.begin(), earlyExploration.end(), [](std::shared_ptr<Node> first, std::shared_ptr<Node> second) { return first->fCost < second->fCost; });
		if (earlyExploration.size() > 0 && earlyExploration.at(0)->fCost <= current->fCost)
		{
			earlyExplorationNode = earlyExploration.at(0);
			earlyExploration.erase(earlyExploration.begin());
		}
		openList.insert(std::end(openList), std::begin(earlyExploration), std::end(earlyExploration));
		earlyExploration.clear();
	}
	
	delete [] closedList;
	return std::vector<Node*>();
}

void Grid::_blockCheck(int x, int y, std::vector<Node*>& targetNodes)
{
	if (m_nodeMap.at(x + y * m_width).tile.getPathable())
	{
		bool canPushBack = true;
		for (auto & t : targetNodes)
		{
			if (m_nodeMap.at(x + y * m_width) == *t)
			{
				canPushBack = false;
				break;
			}
		}

		if (canPushBack)
			targetNodes.push_back(&m_nodeMap.at(x + y * m_width));

		if (targetNodes.size() < MAX_BLOCK_CHECK)
		{
			bool somethingNew = false;
			int startIndex = targetNodes.size();

			auto t = _getUnblockedAround(x, y);

			for (auto newNode : t)
			{
				bool pushThis = true;
				for (auto oldNode : targetNodes)
				{
					if (*newNode == *oldNode)
					{
						pushThis = false;
						break;
					}
				}
				if (pushThis)
				{
					somethingNew = true;
					targetNodes.push_back(newNode);
				}
			}

			if (somethingNew)
			{
				int end = targetNodes.size();
				for (int i = startIndex; i < end; i++)
					_blockCheck(targetNodes.at(i)->tile.getX(), targetNodes.at(i)->tile.getY(), targetNodes);
			}
		}
	}
}

std::vector<Node*> Grid::_getUnblockedAround(int x, int y)
{
	std::vector<Node*> nodes;

	if (x > 0) // Left
		if (m_nodeMap.at(x - 1 + y * m_width).tile.getPathable())
			nodes.push_back(&m_nodeMap.at(x - 1 + y * m_width));
	if (x < m_width - 1) // Right
		if (m_nodeMap.at(x + 1 + y * m_width).tile.getPathable())
			nodes.push_back(&m_nodeMap.at(x + 1 + y * m_width));
	if (y > 0) // Up
		if (m_nodeMap.at(x + (y - 1) * m_width).tile.getPathable())
			nodes.push_back(&m_nodeMap.at(x + (y - 1) * m_width));
	if (y < m_height - 1) // Down
		if (m_nodeMap.at(x + (y + 1) * m_width).tile.getPathable())
			nodes.push_back(&m_nodeMap.at(x + (y + 1) * m_width));
	if (x > 0 && y > 0) // Up left
		if (m_nodeMap.at(x - 1 + (y - 1) * m_width).tile.getPathable())
			nodes.push_back(&m_nodeMap.at(x - 1 + (y - 1) * m_width));
	if (x < m_width - 1 && y > 0) // Up Right
		if (m_nodeMap.at(x + 1 + (y - 1) * m_width).tile.getPathable())
			nodes.push_back(&m_nodeMap.at(x + 1 + (y - 1) * m_width));
	if (x > 0 && y < m_height - 1) // Down Left
		if (m_nodeMap.at(x - 1 + (y + 1) * m_width).tile.getPathable())
			nodes.push_back(&m_nodeMap.at(x - 1 + (y + 1) * m_width));
	if (x < m_width - 1 && y < m_height - 1) // Down Right
		if (m_nodeMap.at(x + 1 + (y + 1) * m_width).tile.getPathable())
			nodes.push_back(&m_nodeMap.at(x + 1 + (y + 1) * m_width));

	return nodes;
}

void Grid::_printTilePairs(std::vector<TilePair> & tilePair, std::ofstream & file, Tile & source, Tile & dest)
{
	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			bool found = false;

			if (m_nodeMap.at(j + i * m_width).tile == source)
			{
				found = true;
				file << "O";
			}
			else if (m_nodeMap.at(j + i * m_width).tile == dest)
			{
				found = true;
				file << "D";
			}
			if (!found)
			{
				int tpCount = 0;
				for (auto & p : tilePair)
				{
					if (m_nodeMap.at(j + i * m_width).tile == p.destination)
					{
						found = true;
						file << tpCount++;
						break;
					}
					else if (m_nodeMap.at(j + i * m_width).tile == p.source)
					{
						found = true;
						file << tpCount++;
						break;
					}
					tpCount++;
				}
			}
			if (!found)
			{
				if (m_nodeMap.at(j + i * m_width).tile.getPathable())
					file << " ";
				else
					file << "#";
			}
			file << " ";
		}
		file << "\n";
	}
	file << "\n";
}

void Grid::_printPath(std::vector<Node*>& path, std::ofstream & file, Tile & source, Tile & dest)
{
	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			bool found = false;

			if (m_nodeMap.at(j + i * m_width).tile == source)
			{
				found = true;
				file << "O";
			}
			else if (m_nodeMap.at(j + i * m_width).tile == dest)
			{
				found = true;
				file << "D";
			}
			if (!found)
				for (auto & p : path)
				{
					if (m_nodeMap.at(j + i * m_width).tile == p->tile)
					{
						found = true;
						file << "X";
						break;
					}
				}
			if (!found)
			{
				if (m_nodeMap.at(j + i * m_width).tile.getPathable())
					file << " ";
				else
					file << "#";
			}
			file << " ";
		}
		file << "\n";
	}

	file << "\n";
}

void Grid::_createSubGrid()
{
	int subGrid = 0;
	
	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			if (m_nodeMap[x + y * m_width].tile.getPathable() && m_nodeMap[x + y * m_width].tile.getSubGrid() == -1)
			{
				std::vector<Node*> targets;
				_subGridCheck(x, y, targets);
				for (auto & n : targets)
					n->tile.setSubGrid(subGrid);

				subGrid++;
			}
		}
	}
}

void Grid::_subGridCheck(int x, int y, std::vector<Node*>& targetNodes)
{
	if (m_nodeMap.at(x + y * m_width).tile.getPathable())
	{
		bool canPushBack = true;
		for (auto & t : targetNodes)
		{
			if (m_nodeMap.at(x + y * m_width) == *t)
			{
				canPushBack = false;
				break;
			}
		}

		if (canPushBack)
			targetNodes.push_back(&m_nodeMap.at(x + y * m_width));

		bool somethingNew = false;
		int startIndex = targetNodes.size();

		auto t = _getUnblockedAround(x, y);

		for (auto newNode : t)
		{
			bool pushThis = true;
			for (auto oldNode : targetNodes)
			{
				if (*newNode == *oldNode)
				{
					pushThis = false;
					break;
				}
			}
			if (pushThis)
			{
				somethingNew = true;
				targetNodes.push_back(newNode);
			}
		}

		if (somethingNew)
		{
			int end = targetNodes.size();
			for (int i = startIndex; i < end; i++)
				_subGridCheck(targetNodes.at(i)->tile.getX(), targetNodes.at(i)->tile.getY(), targetNodes);
		}
	}
}

Node & Node::operator=(const Node & other)
{
	if (this != &other)
	{
		tile = other.tile;
		worldPos = other.worldPos;
		parent = other.parent;
		gCost = other.gCost;
		hCost = other.hCost;
		fCost = other.fCost;
	}
	return *this;
}
