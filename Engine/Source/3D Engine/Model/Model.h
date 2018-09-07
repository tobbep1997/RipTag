#pragma once

#include "Drawable.h"
class Model : public Drawable
{
public:
	Model(ObjectType objectType = ObjectType::Static);
	~Model();


	// Inherited via Drawable

	//Setting the model for the object
	void SetModel(StaticMesh * staticMesh);
	//Setting the model for the object
	void SetModel(DynamicMesh * dynamicMesh);

	/*
	
	setmaterial( Drawable->setMaterial = "wha")

	*/
private:
	void SetBuffer();
};

