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

class RandomRoomGrid;

class Grid
{
private:
	struct TilePair
	{
		Tile source;
		Tile destination;
	};
private:
	std::vector<Node> m_nodeMap;
	std::vector<Node> m_roomNodeMap;
	int m_width, m_height;

	std::vector<Node*> m_path;
	std::future<std::vector<Node*>> m_pathfindingFuture;

public:
	Grid(int width = 0, int height = 0);
	Grid(float xVal, float yVal, int width, int depth);
	virtual ~Grid();

	Tile WorldPosToTile(float x, float y);
	Node GetWorldPosFromIndex(int index);
	void BlockGridTile(int index, bool pathable);

	void CreateGridWithWorldPosValues(ImporterLibrary::GridStruct grid);
	void CreateGridFromRandomRoomLayout(ImporterLibrary::GridStruct grid, int overloaded = 0);
	std::vector<Node*> FindPath(Tile src, Tile dest);
	Tile GetRandomNearbyTile(Tile src);
	//Tile GetRandomNearbyTile(Tile src, int dir = 0);

	void GenerateRoomNodeMap(RandomRoomGrid * randomizer);

	void ThreadPath(Tile src, Tile dest);
	std::vector<Node*> GetPathFromThread();
	bool IsPathReady();
	int getGridWidth();
	int getGridHeight();

private:
	// Utility functions
	void _checkNode(Node * current, float addedGCost, int offsetX, int offsetY, Tile dest,
	std::vector<Node*> & openList, std::vector<Node> & nodeMap, bool * closedList, int width, int height);
	bool _isValid(Tile tile, int width, int height) const;
	float _calcHValue(Tile src, Tile dest) const;
	int _worldPosInNodeMap(int begin, int end, int x, int y) const;
	int _findXInYRow(int begin, int end, int x, int y) const;
	Tile _nearbyTile(Tile src, int x, int y);
	// Should theoretically always return a valid tile in the same room as the source
	Tile _getNearbyUnblockedTile(Tile src);

	bool _tilesAreInTheSameRoom(const Tile & source, const Tile & destination);

	std::vector<Node*> _findRoomNodePath(const Tile & source, const Tile & destination);
	void _removeAllCenterTiles(std::vector<Node*> & roomNodePath);
	Tile _getCenterGridFromRoomGrid(const Tile & tileOnRoomNodeMap, const Tile & tileInNodeMap);

	std::vector<TilePair> _roomNodePathToGridTiles(std::vector<Node*> * roomNodes, const Tile & source, const Tile & destination);

	std::vector<Node*> _findPath(Tile source, Tile destination, std::vector<Node> & nodeMap, int width, int height);
};