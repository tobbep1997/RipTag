#pragma once
#include "Tile.h"
#include <vector>
#include <algorithm>
#include <future>

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
	int m_width, m_height;

	std::vector<Node*> path;
	std::future<std::vector<Node*>> pathfindingFuture;

public:
	Grid(int width = 0, int height = 0);
	virtual ~Grid();

	void ThreadPath(Tile src, Tile dest);
	std::vector<Node*> getPath();
	std::vector<Node*> FindPath(Tile src, Tile dest);

	// Test function
	void printGrid();
	bool Ready();
	
private:
	// Utility functions
	void _checkNode(Node * current, float addedGCost, int offsetX, int offsetY, Tile dest,
			std::vector<Node*> & openList, bool * closedList);
	bool _isValid(Tile tile) const;
	float _calcHValue(Tile src, Tile dest) const;
	
};