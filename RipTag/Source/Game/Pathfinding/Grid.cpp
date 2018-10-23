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
			m_tileMap.push_back(Tile(j, i));
		}
	}
}

Grid::~Grid()
{
}

std::vector<Node*> Grid::FindPath(Tile source, Tile destination)
{
	if (!_isValid(destination))
	{
		std::cout << "Destination is invalid\n";
		return std::vector<Node*>();
	}

	Tile dest = m_tileMap.at(destination.getX() + destination.getY() * m_width);
	if (dest.getBlocked())
	{
		std::cout << "Destination is blocked\n";
		return std::vector<Node*>();
	}

	bool * closedList = new bool[m_height * m_width];
	for (int i = 0; i < m_height * m_width; i++)
		closedList[i] = false;

	std::vector<Node*> openList;

	Node * current = new Node(source, nullptr, 0.0f, _calcHValue(source, dest));
	openList.push_back(current);

	while (!openList.empty())
	{
		int listSize = openList.size() - 1;
		std::sort(openList.begin(), openList.end(), [](Node * first, Node * second) { return first->fCost > second->fCost; });
		current = openList.at(listSize);

		if (current->tile == dest)
		{
			// Do complete path stuff
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
			for (int i = 0; i < listSize; i++)
			{
				delete openList.at(i);
			}

			std::reverse(path.begin(), path.end());
			return path;
		}

		closedList[current->tile.getX() + current->tile.getY() * m_width] = true;
		openList.erase(openList.end() - 1);

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

	delete current;
	delete closedList;
	for (int i = 0; i < openList.size(); i++)
	{
		delete openList.at(i);
	}
	return std::vector<Node*>();
}

void Grid::printGrid()
{
	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			std::cout << m_tileMap.at(j + i * m_width).getBlocked() << " ";
		}
		std::cout << "\n";
	}
}

void Grid::_checkNode(Node * current, float addedGCost, int offsetX, int offsetY, Tile dest, std::vector<Node*> & openList, bool * closedList)
{
	int currentX = current->tile.getX();
	int currentY = current->tile.getY();
	Tile nextTile = Tile(currentX + offsetX, currentY + offsetY);
	int nextTileIndex = nextTile.getX() + nextTile.getY() * m_width;

	if (_isValid(nextTile) && !closedList[nextTileIndex] && !m_tileMap.at(nextTileIndex).getBlocked())
	{
		Node * newNode = new Node(m_tileMap.at(nextTileIndex), current, current->gCost + addedGCost, _calcHValue(nextTile, dest));
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
	int x = src.getX() - dest.getX();
	int y = src.getY() - dest.getY();
	return abs(x) + abs(y);
}