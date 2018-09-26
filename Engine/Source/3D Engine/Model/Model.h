#pragma once

#include "Drawable.h"


class Model : public Drawable
{
private:
	StaticMesh* m_StaticMeshPointer;
	DynamicMesh* m_DynamicMeshPointer;

public:
	Model(ObjectType objectType = ObjectType::Static);
	Model(ObjectType objectType, const std::string &assetFilePath);
	~Model();
	

	//Setting the model for the object
	void setModel(StaticMesh * staticMesh);
	//Setting the model for the object
	void setModel(DynamicMesh * dynamicMesh);
	void setTexture(Texture* textureToset);
	//Set Shaders
	void setVertexShader(const std::wstring & path);
	void setPixelShader(const std::wstring & path);
	void bindTexture(int slot);
	std::string assetName;



	//

private:
	void _setBuffer();
};

