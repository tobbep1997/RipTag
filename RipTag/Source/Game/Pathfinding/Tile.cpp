#include "Tile.h"

Tile::Tile(int _x, int _y, bool _pathable)
{
	m_xCoord = _x;
	m_yCoord = _y;
	m_pathable = _pathable;
}

Tile::~Tile()
{
}

bool Tile::getPathable()
{
	return m_pathable;
}

int Tile::getX()
{
	return m_xCoord;
}

int Tile::getY()
{
	return m_yCoord;
}

void Tile::setPathable(bool _pathable)
{
	m_pathable = _pathable;
}

bool Tile::operator==(Tile & other) const
{
	return m_xCoord == other.m_xCoord && m_yCoord == other.m_yCoord;
}
