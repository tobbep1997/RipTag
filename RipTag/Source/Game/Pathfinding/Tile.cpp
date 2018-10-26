#include "Tile.h"

Tile::Tile(int _x, int _y, bool _blocked)
{
	m_xCoord = _x;
	m_yCoord = _y;
	m_blocked = _blocked;
}

Tile::~Tile()
{
}

bool Tile::getBlocked()
{
	return m_blocked;
}

int Tile::getX()
{
	return m_xCoord;
}

int Tile::getY()
{
	return m_yCoord;
}

void Tile::setBlocked(bool _blocked)
{
	m_blocked = _blocked;
}

bool Tile::operator==(Tile & other) const
{
	return m_xCoord == other.m_xCoord && m_yCoord == other.m_yCoord;
}
