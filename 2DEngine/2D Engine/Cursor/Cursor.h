#pragma once
#include <DirectXMath.h>
#include "../Quad/Quad.h"
class Cursor
{
private:

	Quad * quad;

public:
	Cursor();
	~Cursor();

	void setPos();
};

