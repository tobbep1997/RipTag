#pragma once

#include "Drawable.h"

class Model : public Drawable
{
private:
	StaticMesh* m_StaticMeshPointer;
	DynamicMesh* m_DynamicMeshPointer;

public:
	Model(ObjectType objectType = ObjectType::Static, EntityType ent = EntityType::Defult);
	Model(ObjectType objectType, const std::string &assetFilePath, EntityType ent = EntityType::Defult);
	~Model();
	

	//Setting the model for the object
	void setModel(StaticMesh * staticMesh);
	//Setting the model for the object
	void setModel(DynamicMesh * dynamicMesh);

	//Set Shaders
	void setVertexShader(const std::wstring & path);
	void setPixelShader(const std::wstring & path);



private:
	void _setBuffer();
};

