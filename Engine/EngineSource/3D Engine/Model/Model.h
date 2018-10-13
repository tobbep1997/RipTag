#pragma once

#include "../Components/Base/Drawable.h"


class Model
{
public:
	Model();
	virtual~Model();	

	virtual void setModel(StaticMesh * staticMesh);
	virtual void setModel(DynamicMesh * dynamicMesh);

};

