#pragma once

class Tile
{
private:
	int m_xCoord, m_yCoord;
	bool m_blocked;

public:
	Tile(int _x = -1, int _y = -1, bool _blocked = false);
	virtual ~Tile();

	// Getters
	bool getBlocked();
	int getX();
	int getY();
	
	// Setters
	void setBlocked(bool _blocked);

	// Operators
	bool operator==(Tile & other) const;

private:

};