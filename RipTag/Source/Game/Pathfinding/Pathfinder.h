#pragma once
#include "Grid.h"

struct Node
{
	Node * parent;
	float fCost, gCost, hCost;

	Node(Node * _parent, float _gCost, float _hCost)
	{
		parent = _parent;
		gCost = _gCost;
		hCost = _hCost;
	}
};

class Pathfinder
{
private:

public:

private:

};