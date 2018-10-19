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
			//m_tileGrid.push_back(Tile(j, i));
			m_nodeMap.push_back(Node(Tile(j, i)));
		}
	}
}

Grid::~Grid()
{
}

std::vector<Node> Grid::findPath(Tile source, Tile destination)
{
	Tile dest = m_nodeMap.at(destination.getX() + destination.getY() * m_width).tile;

	if (!_isValid(dest))
	{
		std::cout << "Destination is invalid\n";
		return std::vector<Node>();
	}

	if (dest.getBlocked())
	{
		std::cout << "Destination is blocked\n";
		return std::vector<Node>();
	}

	std::vector<Node> closedList;
	std::vector<Node> openList;

	Node current = Node(source, -1, -1, 0.0f, _calcHValue(source, dest));
	openList.push_back(current);

	while (!openList.empty())
	{
		std::sort(openList.begin(), openList.end(), [](Node first, Node second) { return first.fCost < second.fCost; });
		current = openList.at(0);

		if (current.tile == dest)
		{
			// Do complete path stuff
			std::vector<Node> path;

			while (current.parent_x != -1)
			{
				path.push_back(current);
				current = m_nodeMap.at(current.parent_x + current.parent_y * m_width);
			}

			std::reverse(path.begin(), path.end());
			return path;
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

		// AddedGCost based on the distance to the node, 1.0 for direct paths and 1.414 for diagonal paths.
		// Offset defined by the new tiles direction standing at the source tile.
		/*---------- North ----------*/
		_checkNode(current, 1.0f, 0, -1, dest, openList, closedList);
		/*---------- South ----------*/
		_checkNode(current, 1.0f, 0, 1, dest, openList, closedList);
		/*---------- West ----------*/
		_checkNode(current, 1.0f , -1, 0, dest, openList, closedList);
		/*---------- East ----------*/
		_checkNode(current, 1.0f, 1, 0, dest, openList, closedList);
		/*---------- Northwest ----------*/
		_checkNode(current, 1.414f , -1, -1, dest, openList, closedList);
		/*---------- Northeast ----------*/
		_checkNode(current, 1.414f, 1, -1, dest, openList, closedList);
		/*---------- Southwest ----------*/
		_checkNode(current, 1.414f , -1, 1, dest, openList, closedList);
		/*---------- Southeast ----------*/
		_checkNode(current, 1.414f, 1, 1, dest, openList, closedList);
	}

	return std::vector<Node>();
}

void Grid::printGrid()
{
	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			std::cout << m_nodeMap.at(j + i * m_width).tile.getBlocked() << " ";
		}
		std::cout << "\n";
	}
}

void Grid::blockGrid()
{
	m_nodeMap.at(26).tile.setBlocked(true);
	m_nodeMap.at(260).tile.setBlocked(true);
}

void Grid::_checkNode(Node current, float addedGCost, int offsetX, int offsetY, Tile dest, std::vector<Node> & openList, std::vector<Node> & closedList)
{
	int currentX = current.tile.getX();
	int currentY = current.tile.getY();
	Tile nextTile = Tile(currentX + offsetX, currentY + offsetY);
	int nextTileIndex = nextTile.getX() + nextTile.getY() * m_width;

	if (_isValid(nextTile) && !m_nodeMap.at(nextTileIndex).tile.getBlocked())
	{
		Node newNode = Node(m_nodeMap.at(nextTileIndex).tile, currentX, currentY, current.gCost + addedGCost, _calcHValue(nextTile, dest));
		if (_checkAddToClosedList(closedList, newNode))
		{
			openList.push_back(newNode);
			m_nodeMap.at(nextTileIndex).parent_x = currentX;
			m_nodeMap.at(nextTileIndex).parent_y = currentY;
		}
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
	int x = src.getX() - dest.getX();
	int y = src.getY() - dest.getY();
	return std::max(abs(x), abs(y));
}

/*---------------------------------------------
	Check if the fCost logic works as supposed
---------------------------------------------*/
bool Grid::_checkAddToClosedList(std::vector<Node>& list, Node checkNode) const
{
	for (int i = 0; i < list.size(); i++)
		if (list.at(i) == checkNode && list.at(i).fCost < checkNode.fCost)
			return false;
	return true;
}
