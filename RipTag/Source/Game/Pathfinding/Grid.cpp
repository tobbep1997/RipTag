#include "Grid.h"
// For testing purpose only, remove when finished
#include <iostream>

Grid::Grid(int _width, int _height)
{
	m_width = _width;
	m_height = _height;
	
	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			m_tileGrid.push_back(Tile(j, i));
			//m_nodeMap.push_back(Node());
		}
	}
}

Grid::~Grid()
{
}

std::vector<Node> Grid::findPath(Tile src, Tile dest)
{
	if (!_isValid(dest))
	{
		std::cout << "Destination is invalid\n";
		return;
	}

	if (dest.getBlocked())
	{
		std::cout << "Destination is blocked\n";
		return;
	}

	std::vector<Node> closedList;
	std::vector<Node> openList;

	Node current = Node(src, src.getX(), src.getY(), 0.0f, _calcHValue(src, dest));
	openList.push_back(current);

	while (!openList.empty())
	{
		std::sort(openList.begin(), openList.end(), [](Node first, Node second) { return first.fCost < second.fCost; });
		current = openList.at(0);
		Tile currentTile = current.tile;

		if (currentTile == dest)
		{
			// Do complete path stuff

		}

		closedList.push_back(current);
		openList.erase(openList.begin());

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

		// AddedGCost based on the distance to the node, 1 for direct paths and 1.414 for diagonal paths.
		/*---------- North ----------*/
		_checkNode(currentTile, 0, -1, dest, openList, closedList, current, 1.0f);
		/*---------- South ----------*/
		_checkNode(currentTile, 0, 1, dest, openList, closedList, current, 1.0f);
		/*---------- West ----------*/
		_checkNode(currentTile, -1, 0, dest, openList, closedList, current, 1.0f);
		/*---------- East ----------*/
		_checkNode(currentTile, 1, 0, dest, openList, closedList, current, 1.0f);
		/*---------- Northwest ----------*/
		_checkNode(currentTile, -1, -1, dest, openList, closedList, current, 1.414f);
		/*---------- Northeast ----------*/
		_checkNode(currentTile, 1, -1, dest, openList, closedList, current, 1.414f);
		/*---------- Southwest ----------*/
		_checkNode(currentTile, -1, 1, dest, openList, closedList, current, 1.414f);
		/*---------- Southeast ----------*/
		_checkNode(currentTile, 1, 1, dest, openList, closedList, current, 1.414f);
	}

	return std::vector<Node>();
}

void Grid::_checkNode(Tile checkNextTile, int offsetX, int offsetY, Tile dest, std::vector<Node> & openList, std::vector<Node> & closedList, Node current, float addedGCost)
{
	if (_isValid(checkNextTile, offsetX, offsetY))
	{
		Tile nextTile = Tile(checkNextTile.getX() + offsetX, checkNextTile.getY() + offsetY);
		if (nextTile.getX() == dest.getX() && nextTile.getY() == dest.getY())
		{
			//return true;
		}

		int nextTileIndex = nextTile.getX() + nextTile.getY() * m_width;
		Node newNode = Node(m_tileGrid.at(nextTileIndex), current.tile.getX(), current.tile.getY(), current.gCost + addedGCost, _calcHValue(nextTile, dest));
		if (!m_tileGrid.at(nextTileIndex).getBlocked())
		{
			if (_checkAddToClosedList(closedList, newNode))
			{
				openList.push_back(newNode);
			}
		}
	}
}

bool Grid::_isValid(Tile tile, int offsetX, int offsetY)
{
	int x = tile.getX() + offsetX;
	int y = tile.getY() + offsetY;
	return (x >= 0) && (x < m_width) &&
		(y >= 0) && (y < m_height);
}

float Grid::_calcHValue(Tile src, Tile dest)
{
	int x = src.getX() - dest.getX();
	int y = src.getY() - dest.getY();
	return (float)sqrt((x * x) + (y * y));
}

/*---------------------------------------------
	Check if the fCost logic works as supposed
---------------------------------------------*/
bool Grid::_checkAddToClosedList(std::vector<Node>& list, Node checkNode)
{
	for (int i = 0; i < list.size(); i++)
		if (list.at(i) == checkNode && list.at(i).fCost <= checkNode.fCost)
			return false;
	return true;
}
