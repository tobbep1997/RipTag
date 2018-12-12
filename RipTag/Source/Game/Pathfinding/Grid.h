#pragma once
#include "Tile.h"
#include <vector>
#include <algorithm>
#include <future>
#include <stack>

struct NodeWorldPos
{
	float x, y;

	NodeWorldPos(float _x = -1.0f, float _y = -1.0f)
	{
		x = _x;
		y = _y;
	}
	NodeWorldPos& operator=(const NodeWorldPos & other)
	{
		if (this != &other)
		{
			x = other.x;
			y = other.y;
		}
		return *this;
	}
};

/*
	gCost = the movement cost to move from the starting point to a
		given square on the grid.
	hCost = the estimated movement cost to move from the given square
		on the grid to the final destination.
*/
struct Waypoint;
struct WaypointConnection
{
	Waypoint * connection;
	float connectionCost;
	bool traversedConnection;
	WaypointConnection();

	bool operator==(const WaypointConnection & other);
	
	bool operator==(const Waypoint * other);

	WaypointConnection & operator=(const WaypointConnection & other);
};

struct Waypoint
{
	int x = -1, y = -1;
	int waypointIndex = -1;
	float hCost = FLT_MAX;
	bool visited = false;
	std::vector<WaypointConnection>	connections;

	void AddConnection(const WaypointConnection & wpc)
	{
		connections.push_back(wpc);
	}

	bool operator==(const Waypoint & other)
	{
		return waypointIndex == other.waypointIndex;
	}

	std::vector<Node*> field; // This will be empty in runtime. Only used to find connections
};

struct Node
{
	Waypoint * fieldOwner;
	Tile tile;
	NodeWorldPos worldPos;
	std::shared_ptr<Node> parent;
	float fCost, gCost, hCost;

	Node(Tile _tile = Tile(), NodeWorldPos _worldPos = NodeWorldPos())
	{
		tile = _tile;
		worldPos = _worldPos;
		parent = nullptr;
		gCost = FLT_MAX;
		hCost = FLT_MAX;
		fCost = FLT_MAX;
		fieldOwner = nullptr;
	}

	Node(int _x, int _y, NodeWorldPos _worldPos, std::shared_ptr<Node> _parent, float _gCost, float _hCost)
	{
		tile = Tile(_x, _y);
		worldPos = _worldPos;
		parent = _parent;
		gCost = _gCost;
		hCost = _hCost;
		fCost = gCost + hCost;
		fieldOwner = nullptr;
	}
	
	Node(Tile _tile, NodeWorldPos _worldPos, std::shared_ptr<Node> _parent, float _gCost, float _hCost)
	{
		tile = _tile;
		worldPos = _worldPos;
		parent = _parent;
		gCost = _gCost;
		hCost = _hCost;
		fCost = gCost + hCost;
		fieldOwner = nullptr;
	}

	Node(Node * node)
	{
		tile = node->tile;
		worldPos = node->worldPos;
		parent = node->parent;
		fCost = node->fCost;
		gCost = node->gCost;
		hCost = node->hCost;
		fieldOwner = nullptr;
	}

	bool operator==(Node & other) const { return tile == other.tile; }
	Node& operator=(const Node & other);
};

class RandomRoomGrid;

class Grid
{
private:
	static const int FIELD_LIMIT = 64;
	struct TilePair
	{
		Tile source;
		Tile destination;
	};
private:
	const int MAX_BLOCK_CHECK = 21;
	std::vector<Node> m_nodeMap;
	std::vector<Node> m_roomNodeMap;
	std::vector<Waypoint> m_waypoints;
	int m_width, m_height;

public:
	Grid(int width = 0, int height = 0);
	Grid(float xVal, float yVal, int width, int depth);
	virtual ~Grid();

	Tile WorldPosToTile(float x, float y);
	Node GetWorldPosFromIndex(int index);
	void BlockGridTile(int index, bool pathable);
	
	void CreateGridWithWorldPosValues(ImporterLibrary::GridStruct grid);
	void CreateGridFromRandomRoomLayout(ImporterLibrary::GridStruct grid);
	void GenerateRoomNodeMap(RandomRoomGrid * randomizer);
	std::vector<Node*> FindPath(Tile src, Tile dest);
	// Should theoretically always return a valid tile in the same room as the source
	Tile GetRandomNearbyUnblockedTile(Tile src);	
	
	Node * GetNodeAt(int index);

	int getGridWidth();
	int getGridHeight();

	// For Blocking Algorithm;
	void BlockIfNotPathable(int targetX, int targetY);

	void PrintMe() const;

private:
	// Utility functions
	void _checkNode(std::shared_ptr<Node> current, float addedGCost, int offsetX, int offsetY, Tile dest,
					std::vector<std::shared_ptr<Node>>& openList, std::vector<Node> & nodeMap,
					bool * closedList, int width, int height);
	const bool _isValid(Tile tile, int width, int height) const;
	const float _calcHValue(Tile src, Tile dest) const;
	int _worldPosInNodeMap(int begin, int end, int x, int y) const;
	int _findXInYRow(int begin, int end, int x, int y) const;
	const bool _tilesAreInTheSameRoom(const Tile & source, const Tile & destination);
	std::vector<Node*> _findRoomNodePath(const Tile & source, const Tile & destination);
	const bool _isAllDirectionsBlocked(const bool blockedDirections[8]);
	void _removeAllBlockedTiles(std::vector<Node*> & roomNodePath);
	Tile _getCenterGridFromRoomGrid(const Tile & tileOnRoomNodeMap, const Tile & tileInNodeMap);
	std::vector<TilePair> _roomNodePathToGridTiles(std::vector<Node*> * roomNodes, const Tile & source, const Tile & destination);
	std::vector<Node*> _findPath(Tile source, Tile destination, std::vector<Node> & nodeMap, int width, int height);
	std::vector<Waypoint*> _findWaypointPath(const Tile & source, const Tile & destination);
	std::vector<TilePair> _waypointPathToGridTiles(std::vector<Waypoint*>& waypoints, const Tile & source, const Tile & destination);

	// Waypoint functions
	Waypoint _createField(int x, int y, bool * visitedNodes);
	void _findConnections();
	void _generateWaypoints();
	void _includeInField(int x, int y, bool * visitedNodes, std::vector<Node*> & targets, int startIndex = 0);

	// For Blocking Algorithm;
	void				_blockCheck(int x, int y, std::vector<Node*> &targetNodes);
	std::vector<Node*>	_getUnblockedAround(int x, int y);

	void _printTilePairs(std::vector<TilePair> & tilePair, std::ofstream & file, Tile & source, Tile & dest);
	void _printPath(std::vector<Node*> & path, std::ofstream & file, Tile & source, Tile & dest);
	void _createSubGrid();
	void _subGridCheck(int x, int y, std::vector<Node*> &targetNodes);
};