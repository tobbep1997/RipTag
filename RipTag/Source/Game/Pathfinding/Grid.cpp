//#ifdef _DEBUG
//#ifndef DBG_NEW
//#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
//#define new DBG_NEW
//#endif
//#endif  // _DEBUG

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
	for (auto asset : m_path)
	{
		delete asset;
	}
	m_path.clear();


}

Tile Grid::WorldPosToTile(float x, float y)
{
	int approximateWorldPosX = (int)x;
	int approximateWorldPosY = (int)y;
	int index = _worldPosInNodeMap(0, m_height * m_width - 1, approximateWorldPosX, approximateWorldPosY);

	if (index == -1)
		return Tile();

	return m_nodeMap.at(index).tile;
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
	
	if (!m_nodeMap.empty())
		m_nodeMap.clear();
	
	if (grid.gridPoints)
	{
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
	}
}

void Grid::CreateGridFromRandomRoomLayout(ImporterLibrary::GridStruct grid, int overloaded)
{
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

std::vector<Node*> Grid::FindPath(Tile source, Tile destination)
{
	if (!m_roomNodeMap.empty() && !_tilesAreInTheSameRoom(source, destination))
	{
		std::vector<Node*> pathToDestination;
		
		// A* through the "large" grid to find which rooms are connected in the path
		std::vector<Node*> roomNodePath = _findRoomNodePath(source, destination);

		_removeAllCenterTiles(roomNodePath);

		// A* in each room to get to the next
		std::vector<TilePair> tilePairs = _roomNodePathToGridTiles(&roomNodePath, source, destination);
		

		for (auto & tp : tilePairs)
		{
			auto partOfPath = _findPath(tp.source, tp.destination, m_nodeMap, m_width, m_height);
			pathToDestination.insert(std::end(pathToDestination), std::begin(partOfPath), std::end(partOfPath));
		}
		
		// Merge the paths
		
		for (auto & p : roomNodePath)
			delete p;
		roomNodePath.clear();

		return pathToDestination;
	}
	else
	{
		return _findPath(source, destination, m_nodeMap, m_width, m_height);
	}
}

Tile Grid::GetRandomNearbyTile(Tile src, int dir)
{
	if (src.getX() != -1)
	{
		// Make random
		int direction = dir;
		int x = src.getX();
		int y = src.getY();
		int index = 0;

		switch (direction)
		{
			// North
		case 0:
			index = x - m_width + y * m_width;
			if (index >= 0 && m_nodeMap.at(index).tile.getPathable())
				return _nearbyTile(src, 0, -1);
			// East
		case 1:
			index = x + 1 + y * m_width;
			if (index < m_width + y * m_width && m_nodeMap.at(index).tile.getPathable())
				return _nearbyTile(src, 1, 0);
			// South
		case 2:
			index = x + m_width + y * m_width;
			if (index < m_nodeMap.size() && m_nodeMap.at(index).tile.getPathable())
				return _nearbyTile(src, 0, 1);
		case 3:
			index = x - 1 + y * m_width;
			if (index >= y * m_width && m_nodeMap.at(index).tile.getPathable())
				return _nearbyTile(src, -1, 0);
			break;
		}
	}
	return Tile();
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
			bool pathable = true;
			if (i % 2 == 1)
				if (j % 2 == 1)
					pathable = false;
			m_roomNodeMap.push_back(Node(Tile(j, i, pathable), NodeWorldPos(worldX + j * 10.0f, worldY + i * 10.0f)));
		}
	}
	int counter = 0;
	for (int i = 0; i < depth; i += 2)
	{
		for (int j = 0; j < width; j += 2)
		{
			int index = counter++;
			if (i < depth - 1)
				m_roomNodeMap[j + (i + 1) * width].tile.setPathable(randomizer->m_rooms[index].south);
			if (j < width - 1)
				m_roomNodeMap[(j + 1) + i * width].tile.setPathable(randomizer->m_rooms[index].east);
		}
	}

	// Transpose
	/*for (int i = 0; i < depth; i++)
	{
		for (int j = i; j < width; j++)
		{
			int currentIndex = j + i * width;
			int transposeIndex = i + j * width;
			Node temp = m_roomNodeMap[currentIndex];
			m_roomNodeMap[currentIndex] = m_roomNodeMap[transposeIndex];
			m_roomNodeMap[transposeIndex] = temp;
		}
	}*/

}

void Grid::ThreadPath(Tile src, Tile dest)
{
	m_pathfindingFuture = std::async(std::launch::async, &Grid::FindPath, this, src, dest);
}

std::vector<Node*> Grid::GetPathFromThread()
{
	m_path = m_pathfindingFuture.get();
	return m_path;
}

bool Grid::IsPathReady()
{
	using namespace std::chrono_literals;
	auto status = m_pathfindingFuture.wait_for(0s);
	return status == std::future_status::ready;
}

int Grid::getGridWidth()
{
	return m_width;
}

int Grid::getGridHeight()
{
	return m_height;
}

void Grid::Transpose()
{/*
	for (int i = 0; i < m_height; i++)
		for (int j = i; j < m_width; j++)
		{
			int currentIndex = j + i * m_width;
			int transposeIndex = i + j * m_width;
			Node temp = m_nodeMap[currentIndex];
			m_nodeMap[currentIndex] = m_nodeMap[transposeIndex];
			m_nodeMap[transposeIndex] = temp;
		}*/
}

bool Grid::isBlocked(int index) const
{
	return !m_nodeMap.at(index).tile.getPathable();
}

const std::vector<Node>* Grid::getRoomNodeMap() const
{
	return &m_roomNodeMap;
}

const std::vector<Node>* Grid::getNodeMap() const
{
	return &m_nodeMap;
}

void Grid::_checkNode(Node * current, float addedGCost, int offsetX, int offsetY, Tile dest, std::vector<Node*> & openList,
	std::vector<Node> & nodeMap, bool * closedList, int width, int height)
{
	int currentX = current->tile.getX();
	int currentY = current->tile.getY();
	Tile nextTile = Tile(currentX + offsetX, currentY + offsetY);
	int nextTileIndex = nextTile.getX() + nextTile.getY() * width;

	if (_isValid(nextTile, width, height) && !closedList[nextTileIndex] && nodeMap.at(nextTileIndex).tile.getPathable())
	{
		Node * newNode = DBG_NEW Node(nodeMap.at(nextTileIndex).tile, nodeMap.at(nextTileIndex).worldPos,
			current, current->gCost + addedGCost, _calcHValue(nextTile, dest));
		openList.push_back(newNode);
	}
}

bool Grid::_isValid(Tile tile, int width, int height) const
{
	int x = tile.getX();
	int y = tile.getY();
	return (x >= 0) && (x < width) &&
		(y >= 0) && (y < height);
}

float Grid::_calcHValue(Tile src, Tile dest) const
{
	int x = abs(src.getX() - dest.getX());
	int y = abs(src.getY() - dest.getY());
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

Tile Grid::_nearbyTile(Tile src, int x, int y)
{
	bool foundTile = false;
	Tile destination = Tile(src.getX() + x, src.getY() + y);
	int destX = destination.getX();
	int destY = destination.getY();
	int count = 0;

	while (!foundTile)
	{
		if (count > 4 || !m_nodeMap.at(destX + destY * m_width).tile.getPathable())
		{
			foundTile = true;
			destX -= x;
			destY -= y;
		}
		else
		{
			count++;
			destX += x;
			destY += y;
		}
	}
	/*destination = Tile(destination.getX() + x * count, destination.getY() + y * count);
	destX = destination.getX();
	destY = destination.getY();*/
	if (destX < 0)
		destX = 0;
	if (destY < 0)
		destY = 0;
	if (destX < y * m_width)
		destX = m_width * y - 1;
	if (destY >= m_height)
		destY = m_height - 1;
	destination = Tile(destX, destY);

	return destination;
}

Tile Grid::_getNearestUnblockedTile(int x, int y)
{
	bool gotTile = false;
	int stepSize = 1;

	while (!gotTile)
	{
		/*---------- North ----------*/
		//(0, -1);
		if (m_nodeMap[x + (y - 1) * x].tile.getPathable())
		{

		}
		/*---------- South ----------*/
		//(0, 1);
		if (m_nodeMap[x + (y + 1) * x].tile.getPathable())
		{

		}
		/*---------- West ----------*/
		//(-1, 0);
		if (m_nodeMap[(x - 1) + y * x].tile.getPathable())
		{

		}
		/*---------- East ----------*/
		//(1, 0);
		if (m_nodeMap[(x + 1) + y * x].tile.getPathable())
		{

		}
		/*---------- Northwest ----------*/
		//(-1, -1);
		if (m_nodeMap[(x - 1) + (y - 1) * x].tile.getPathable())
		{

		}
		/*---------- Northeast ----------*/
		//(1, -1);
		if (m_nodeMap[(x + 1) + (y - 1) * x].tile.getPathable())
		{

		}
		/*---------- Southwest ----------*/
		//(-1, 1);
		if (m_nodeMap[(x - 1) + (y + 1) * x].tile.getPathable())
		{

		}
		/*---------- Southeast ----------*/
		//(1, 1);
		if (m_nodeMap[(x + 1) + (y + 1) * x].tile.getPathable())
		{

		}
	}

	return Tile();
}

bool Grid::_tilesAreInTheSameRoom(const Tile & source, const Tile & destination)
{
	using namespace DirectX;
	NodeWorldPos sWpos = m_nodeMap[source.getX() + source.getY() * m_width].worldPos;
	NodeWorldPos dWpos = m_nodeMap[destination.getX() + destination.getY() * m_width].worldPos;

	XMFLOAT2 sWposOff = { sWpos.x + 50.0f, sWpos.y + 50.0f };
	XMFLOAT2 dWposOff = { dWpos.x + 50.0f, dWpos.y + 50.0f };

	XMINT2 sIndex2D = { (int)sWposOff.x / 20, (int)sWposOff.y / 20 };
	XMINT2 dIndex2D = { (int)dWposOff.x / 20, (int)dWposOff.y / 20 };

	const int ROOM_WIDTH = 5;

	sIndex2D.x = max(sIndex2D.x, 0);
	sIndex2D.y = max(sIndex2D.y, 0);
	sIndex2D.x = min(sIndex2D.x, 5);
	sIndex2D.y = min(sIndex2D.y, 5);

	dIndex2D.x = max(dIndex2D.x, 0);
	dIndex2D.y = max(dIndex2D.y, 0);
	dIndex2D.x = min(dIndex2D.x, 5);
	dIndex2D.y = min(dIndex2D.y, 5);

	int sIndex = sIndex2D.y * ROOM_WIDTH + sIndex2D.x;
	int dIndex = dIndex2D.y * ROOM_WIDTH + dIndex2D.x;

	return sIndex == dIndex;
}

std::vector<Node*> Grid::_findRoomNodePath(const Tile & source, const Tile & destination)
{
	using namespace DirectX;
	NodeWorldPos sWpos = m_nodeMap[source.getX() + source.getY() * m_width].worldPos;
	NodeWorldPos dWpos = m_nodeMap[destination.getX() + destination.getY() * m_width].worldPos;

	XMFLOAT2 sWposOff = { sWpos.x + 40.0f, sWpos.y + 40.0f };
	XMFLOAT2 dWposOff = { dWpos.x + 40.0f, dWpos.y + 40.0f };

	XMINT2 sIndex2D = { (int)sWposOff.x / 10, (int)sWposOff.y / 10 };
	XMINT2 dIndex2D = { (int)dWposOff.x / 10, (int)dWposOff.y / 10 };

	const int ROOM_WIDTH = 9;

	sIndex2D.x = max(sIndex2D.x, 0);
	sIndex2D.y = max(sIndex2D.y, 0);
	sIndex2D.x = min(sIndex2D.x, 8);
	sIndex2D.y = min(sIndex2D.y, 8);

	dIndex2D.x = max(dIndex2D.x, 0);
	dIndex2D.y = max(dIndex2D.y, 0);
	dIndex2D.x = min(dIndex2D.x, 8);
	dIndex2D.y = min(dIndex2D.y, 8);

	int sIndex = sIndex2D.y * ROOM_WIDTH + sIndex2D.x;
	int dIndex = dIndex2D.y * ROOM_WIDTH + dIndex2D.x;

	// TODO :: Translate this to center point of the room (sIndex and dIndex)

	Tile roomSource = m_roomNodeMap[sIndex].tile;
	Tile roomDest = m_roomNodeMap[dIndex].tile;

	roomSource = _getCenterGridFromRoomGrid(roomSource, source);

	roomDest = _getCenterGridFromRoomGrid(roomDest, destination);

	return _findPath(roomSource, roomDest, m_roomNodeMap, 9, 9);
}

void Grid::_removeAllCenterTiles(std::vector<Node*>& roomNodePath)
{
	int size = roomNodePath.size();

	for (int i = 0; i < size; i++)
	{
		if (roomNodePath[i]->tile.getX() % 2 == 0 && roomNodePath[i]->tile.getY() % 2 == 0 ||
			!WorldPosToTile(roomNodePath[i]->worldPos.x, roomNodePath[i]->worldPos.y).getPathable())
		{
			// This is a center tile
			roomNodePath.erase(roomNodePath.begin() + i);
		}
		size = roomNodePath.size();
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

	auto sWpos = m_nodeMap[tileInNodeMap.getX() + tileInNodeMap.getY() * 9].worldPos;
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

std::vector<Grid::TilePair> Grid::_roomNodePathToGridTiles(std::vector<Node*>* roomNodes, const Tile & source, const Tile & destination)
{
	std::vector<Grid::TilePair> gtp;

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

	if (!dest.getPathable())
		return std::vector<Node*>();

	bool * closedList = new bool[height * width];
	for (int i = 0; i < height * width; i++)
		closedList[i] = false;

	std::vector<Node*> openList;
	std::vector<Node*> earlyExploration;

	Node * earlyExplorationNode = nullptr;
	Node * current = new Node(source, NodeWorldPos(), nullptr, 0.0f, _calcHValue(source, dest));
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
			std::sort(openList.begin(), openList.end(), [](Node * first, Node * second) { return first->fCost < second->fCost; });
			current = openList.at(0);
			
			openList.erase(openList.begin());
		}

		if (current->tile == dest)
		{
			// Create complete path
			std::vector<Node*> path;

			// Creates the new path and deletes the pointers that are used to create the path
			while (current->parent != nullptr)
			{
				Node * nextPathNode = new Node(current);
				path.push_back(nextPathNode);
				delete current;
				current = nextPathNode->parent;
			}

			// Deletes any spare grid pointers
			delete current;
			delete closedList;
			for (int i = 0; i < openList.size(); i++)
			{
				delete openList.at(i);
			}

			std::reverse(path.begin(), path.end());
			return path;
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

		std::sort(earlyExploration.begin(), earlyExploration.end(), [](Node * first, Node * second) { return first->fCost < second->fCost; });
		if (earlyExploration.size() > 0 && earlyExploration.at(0)->fCost <= current->fCost)
		{
			earlyExplorationNode = earlyExploration.at(0);
			earlyExploration.erase(earlyExploration.begin());
		}
		openList.insert(std::end(openList), std::begin(earlyExploration), std::end(earlyExploration));
		earlyExploration.clear();
	}

	delete current;
	delete closedList;
	for (int i = 0; i < openList.size(); i++)
	{
		delete openList.at(i);
	}
	return std::vector<Node*>();
}
