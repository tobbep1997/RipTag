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
struct NodeWorldPos
{
	float x, y;

	NodeWorldPos(float _x = -1.0f, float _y = -1.0f)
	{
		x = _x;
		y = _y;
	}
};

struct Node
{
	Tile tile;
	NodeWorldPos worldPos;
	Node * parent;
	float fCost, gCost, hCost;

	Node(Tile _tile = Tile(), NodeWorldPos _worldPos = NodeWorldPos())
	{
		tile = _tile;
		worldPos = _worldPos;
		parent = nullptr;
		gCost = FLT_MAX;
		hCost = FLT_MAX;
		fCost = FLT_MAX;
	}

	Node(int _x, int _y, NodeWorldPos _worldPos, Node * _parent, float _gCost, float _hCost)
	{
		tile = Tile(_x, _y);
		worldPos = _worldPos;
		parent = _parent;
		gCost = _gCost;
		hCost = _hCost;
		fCost = gCost + hCost;
	}
	
	Node(Tile _tile, NodeWorldPos _worldPos, Node * _parent, float _gCost, float _hCost)
	{
		tile = _tile;
		worldPos = _worldPos;
		parent = _parent;
		gCost = _gCost;
		hCost = _hCost;
		fCost = gCost + hCost;
	}

	Node(Node * node)
	{
		tile = node->tile;
		worldPos = node->worldPos;
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
	std::vector<Node> m_nodeMap;
	int m_width, m_height;

	//std::vector<Node*> m_path;
	//std::future<std::vector<Node*>> m_pathfindingFuture;

public:
	Grid(int width = 0, int height = 0);
	Grid(float xVal, float yVal, int width, int depth);
	virtual ~Grid();

	Tile WorldPosToTile(float x, float y);

	void CreateGridWithWorldPosValues(ImporterLibrary::GridStruct grid);
	std::vector<Node*> FindPath(Tile src, Tile dest);

	//void ThreadPath(Tile src, Tile dest);
	//std::vector<Node*> getPath();
	//bool Ready();

	// Test function
	void printGrid();
	void printWorldPos();

private:
	// Utility functions
	void _checkNode(Node * current, float addedGCost, int offsetX, int offsetY, Tile dest,
			std::vector<Node*> & openList, bool * closedList);
	bool _isValid(Tile tile) const;
	float _calcHValue(Tile src, Tile dest) const;
	int _worldPosInNodeMap(int begin, int end, int x, int y) const;
	int _findXInYRow(int begin, int end, int x, int y) const;
};