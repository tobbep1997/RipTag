#pragma once

#include "Drawable.h"

class Model : public Drawable
{
public:
	Model(ObjectType objectType = ObjectType::Static);
	~Model();

	//Setting the model for the object
	void SetModel(StaticMesh * staticMesh);
	//Setting the model for the object
	void SetModel(DynamicMesh * dynamicMesh);

	//Set Shaders
	void SetVertexShader(const std::wstring & path);
	void SetPixelShader(const std::wstring & path);

private:
	void SetBuffer();
};

