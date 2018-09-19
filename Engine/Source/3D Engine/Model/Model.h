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
	void setTexture(const std::string & filepath);
	//Set Shaders
	void SetVertexShader(const std::wstring & path);
	void SetPixelShader(const std::wstring & path);
	std::string assetName;

private:
	void SetBuffer();
	

	//

	Texture m_texture;
	StaticMesh* m_StaticMeshPointer;
	DynamicMesh* m_DynamicMeshPointer;

};

