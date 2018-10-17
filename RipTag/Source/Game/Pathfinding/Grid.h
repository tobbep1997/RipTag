#pragma once
#include "Tile.h"
#include <vector>
#include <algorithm>

/*
	gCost = the movement cost to move from the starting point to a
		given square on the grid.
	hCost = the estimated movement cost to move from the given square
		on the grid to the final destination.
*/
struct Node
{
	Tile tile;
	int parent_x, parent_y;
	float fCost, gCost, hCost;

	Node()
	{
		tile = Tile();
		parent_x = -1;
		parent_y = -1;
		gCost = 1000000.0f;
		hCost = 1000000.0f;
		fCost = 1000000.0f;
	}

	Node(int _x, int _y, int _parent_x, int _parent_y, float _gCost, float _hCost)
	{
		tile = Tile(_x, _y);
		parent_x = _parent_x;
		parent_y = _parent_y;
		gCost = _gCost;
		hCost = _hCost;
		fCost = gCost + hCost;
	}
	
	Node(Tile _tile, int _parent_x, int _parent_y, float _gCost, float _hCost)
	{
		tile = _tile;
		parent_x = _parent_x;
		parent_y = _parent_y;
		gCost = _gCost;
		hCost = _hCost;
		fCost = gCost + hCost;
	}
	
	bool operator==(Node & other) const { return tile == other.tile; }
};

class Grid
{
private:
	std::vector<Tile> m_tileGrid;
	//std::vector<Node> m_nodeMap;
	int m_width, m_height;

public:
	Grid(int _width = 0, int _height = 0);
	virtual ~Grid();

	std::vector<Node> findPath(Tile src, Tile dest);
	
private:
	// Utility functions
	void _checkNode(Tile checkNextTile, int offsetX, int offsetY, Tile dest,
			std::vector<Node> & openList, std::vector<Node> & closedList, Node current, float addedGCost);
	bool _isValid(Tile tile, int offsetX = 0, int offsetY = 0);
	float _calcHValue(Tile src, Tile dest);
	bool _checkAddToClosedList(std::vector<Node> & list, Node checkNode);
	
};