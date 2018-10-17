#pragma once
#include "Grid.h"

struct Node
{
	Tile tile;
	float fCost, gCost, hCost;

	Node(int _x, int _y, float _gCost, float _hCost)
	{
		tile = Tile(_x, _y);
		gCost = _gCost;
		hCost = _hCost;
		fCost = gCost + hCost;
	}
	Node(Tile _tile, float _gCost, float _hCost)
	{
		tile = _tile;
		gCost = _gCost;
		hCost = _hCost;
		fCost = gCost + hCost;
	}
};

class Pathfinder
{
private:

public:

private:

};