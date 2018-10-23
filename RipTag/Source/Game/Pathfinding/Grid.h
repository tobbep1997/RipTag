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
	Node * parent;
	float fCost, gCost, hCost;

	Node(Tile _tile = Tile())
	{
		tile = _tile;
		parent = nullptr;
		gCost = FLT_MAX;
		hCost = FLT_MAX;
		fCost = FLT_MAX;
	}

	Node(int _x, int _y, Node * _parent, float _gCost, float _hCost)
	{
		tile = Tile(_x, _y);
		parent = _parent;
		gCost = _gCost;
		hCost = _hCost;
		fCost = gCost + hCost;
	}
	
	Node(Tile _tile, Node * _parent, float _gCost, float _hCost)
	{
		tile = _tile;
		parent = _parent;
		gCost = _gCost;
		hCost = _hCost;
		fCost = gCost + hCost;
	}

	Node(Node * node)
	{
		tile = node->tile;
		parent = node->parent;
		fCost = node->fCost;
		gCost = node->gCost;
		hCost = node->hCost;
	}

	bool operator==(Node & other) const { return tile == other.tile; }
};

class Grid
{
private:
	std::vector<Tile> m_tileMap;
	//std::vector<Node> m_nodeMap;
	int m_width, m_height;

public:
	Grid(int _width = 0, int _height = 0);
	virtual ~Grid();

	std::vector<Node*> FindPath(Tile src, Tile dest);

	// Test function
	void printGrid();
	
private:
	// Utility functions
	void _checkNode(Node * current, float addedGCost, int offsetX, int offsetY, Tile dest,
			std::vector<Node*> & openList, bool * closedList);
	bool _isValid(Tile tile) const;
	float _calcHValue(Tile src, Tile dest) const;
	
};