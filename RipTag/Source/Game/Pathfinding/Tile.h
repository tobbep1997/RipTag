#pragma once

class Tile
{
private:
	int m_xCoord, m_yCoord;
	int m_subGrid;
	bool m_pathable;

public:
	Tile(int _x = -1, int _y = -1, bool _pathable = true);
	virtual ~Tile();

	// Getters
	bool getPathable() const;
	int getX() const;
	int getY() const;
	int getSubGrid() const;

	// Setters
	void setPathable(bool _pathable);
	void setSubGrid(int sub);

	// Operators
	bool operator==(Tile & other) const;
	Tile & operator=(const Tile & other);

private:

};