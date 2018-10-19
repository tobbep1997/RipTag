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

	Node(Tile _tile = Tile())
	{
		tile = _tile;
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
	//std::vector<Tile> m_tileGrid;
	std::vector<Node> m_nodeMap;
	int m_width, m_height;

public:
	Grid(int _width = 0, int _height = 0);
	virtual ~Grid();

	std::vector<Node> findPath(Tile src, Tile dest);

	// Test function
	void printGrid();
	void blockGrid();
	
private:
	// Utility functions
	void _checkNode(Node current, float addedGCost, int offsetX, int offsetY, Tile dest,
			std::vector<Node> & openList, std::vector<Node> & closedList);
	bool _isValid(Tile tile) const;
	float _calcHValue(Tile src, Tile dest) const;
	bool _checkAddToClosedList(std::vector<Node> & list, Node checkNode) const;
	
};