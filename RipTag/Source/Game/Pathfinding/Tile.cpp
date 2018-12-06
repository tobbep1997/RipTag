#include "RipTagPCH.h"
#include "Tile.h"

Tile::Tile(int _x, int _y, bool _pathable)
{
	m_xCoord = _x;
	m_yCoord = _y;
	m_pathable = _pathable;
	m_subGrid = -1;
}

Tile::~Tile()
{
}

bool Tile::getPathable() const
{
	return m_pathable;
}

int Tile::getX() const
{
	return m_xCoord;
}

int Tile::getY() const
{
	return m_yCoord;
}

int Tile::getSubGrid() const
{
	return m_subGrid;
}

void Tile::setPathable(bool _pathable)
{
	m_pathable = _pathable;
}

void Tile::setSubGrid(int sub)
{
	m_subGrid = sub;
}

bool Tile::operator==(Tile & other) const
{
	return m_xCoord == other.m_xCoord && m_yCoord == other.m_yCoord;
}

Tile & Tile::operator=(const Tile & other)
{
	if (this != &other)
	{
		m_xCoord = other.m_xCoord;
		m_yCoord = other.m_yCoord;
		m_pathable = other.m_pathable;
		m_subGrid = other.m_subGrid;
	}
	return *this;
}
