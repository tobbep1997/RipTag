#pragma once

#include "Drawable.h"


class Model : public Drawable
{
public:
	Model(ObjectType objectType = ObjectType::Static);
	
	Model(ObjectType objectType, const std::string &assetFilePath);
	~Model();

	//Setting the model for the object
	void SetModel(StaticMesh * staticMesh);
	//Setting the model for the object
	void SetModel(DynamicMesh * dynamicMesh);
	void setTexture(Texture* textureToset);
	//Set Shaders
	void SetVertexShader(const std::wstring & path);
	void SetPixelShader(const std::wstring & path);
	std::string assetName;
	
	void bindTexture(int slot);

private:
	void SetBuffer();
	

	//

	StaticMesh* m_StaticMeshPointer;
	DynamicMesh* m_DynamicMeshPointer;

};

