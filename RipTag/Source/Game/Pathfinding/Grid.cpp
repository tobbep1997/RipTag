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

void Grid::CreateGridWithWorldPosValues(ImporterLibrary::GridStruct grid)
{
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
}

void Grid::CreateGridFromRandomRoomLayout(ImporterLibrary::GridStruct grid, int roomWidth, int roomSize)
{
	m_nodeMap.clear();
	m_width = grid.maxX;
	m_height = grid.maxY;
	int iterations = (m_height * m_width) / roomSize;

	for (int i = 0; i < iterations; i++)
		for (int j = 0; j < roomWidth; j++)
		{
			int index = j * roomWidth + i * roomSize;
			m_nodeMap.push_back(Node(Tile(j, i, grid.gridPoints[index].pathable),
				NodeWorldPos(grid.gridPoints[index].translation[0],
					grid.gridPoints[index].translation[2])));
		}
}

std::vector<Node*> Grid::FindPath(Tile source, Tile destination)
{
	if (!_isValid(destination) || !_isValid(source))
		return std::vector<Node*>();

	Tile dest = m_nodeMap.at(destination.getX() + destination.getY() * m_width).tile;
	if (!dest.getPathable())
		return std::vector<Node*>();

	bool * closedList = new bool[m_height * m_width];
	for (int i = 0; i < m_height * m_width; i++)
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
		closedList[current->tile.getX() + current->tile.getY() * m_width] = true;

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
		_checkNode(current, 1.0f, 0, -1, dest, earlyExploration, closedList);
		/*---------- South ----------*/
		_checkNode(current, 1.0f, 0, 1, dest, earlyExploration, closedList);
		/*---------- West ----------*/
		_checkNode(current, 1.0f , -1, 0, dest, earlyExploration, closedList);
		/*---------- East ----------*/
		_checkNode(current, 1.0f, 1, 0, dest, earlyExploration, closedList);
		/*---------- Northwest ----------*/
		_checkNode(current, 1.414f , -1, -1, dest, earlyExploration, closedList);
		/*---------- Northeast ----------*/
		_checkNode(current, 1.414f, 1, -1, dest, earlyExploration, closedList);
		/*---------- Southwest ----------*/
		_checkNode(current, 1.414f , -1, 1, dest, earlyExploration, closedList);
		/*---------- Southeast ----------*/
		_checkNode(current, 1.414f, 1, 1, dest, earlyExploration, closedList);

		std::sort(earlyExploration.begin(), earlyExploration.end(), [](Node * first, Node * second) { return first->fCost < second->fCost; });
		if (earlyExploration.size() > 0 && earlyExploration.at(0)->fCost <= current->fCost)
		{
			earlyExplorationNode = earlyExploration.at(0);
			earlyExploration.erase(earlyExploration.begin());
		}
		else
		{
			// Might be needed to avoid mem leaks if it doesn't lead to any new paths.
			delete current;
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

std::vector<Node*> Grid::InvestigateAreaPath(Tile src)
{
	return std::vector<Node*>();
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

void Grid::_checkNode(Node * current, float addedGCost, int offsetX, int offsetY, Tile dest, std::vector<Node*> & openList, bool * closedList)
{
	int currentX = current->tile.getX();
	int currentY = current->tile.getY();
	Tile nextTile = Tile(currentX + offsetX, currentY + offsetY);
	int nextTileIndex = nextTile.getX() + nextTile.getY() * m_width;

	if (_isValid(nextTile) && !closedList[nextTileIndex] && m_nodeMap.at(nextTileIndex).tile.getPathable())
	{
		Node * newNode = new Node(m_nodeMap.at(nextTileIndex).tile, m_nodeMap.at(nextTileIndex).worldPos,
			current, current->gCost + addedGCost, _calcHValue(nextTile, dest));
		openList.push_back(newNode);
	}
}

bool Grid::_isValid(Tile tile) const
{
	int x = tile.getX();
	int y = tile.getY();
	return (x >= 0) && (x < m_width) &&
		(y >= 0) && (y < m_height);
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