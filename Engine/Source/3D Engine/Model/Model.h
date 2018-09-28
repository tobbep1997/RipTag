#pragma once

#include "../Components/Drawable.h"


class Model : public Drawable
{
public:
	Model();
	virtual~Model();	

	virtual void setModel(StaticMesh * staticMesh);
	virtual void setModel(DynamicMesh * dynamicMesh);
	std::string assetName;

private:
	virtual void _setBuffer();
};

