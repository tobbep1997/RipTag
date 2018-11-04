#pragma once
#include <vector>
class Drawable;
class Texture;
class StaticMesh;
class DynamicMesh;

class ModelManager
{
private:
	

public:
	ModelManager();
	~ModelManager();
	
	std::vector<Drawable*> m_staticModel;
	std::vector<Drawable*> m_dynamicModel;
	
	void DrawMeshes();
	void addNewModel(StaticMesh* mesh, Texture* texture);
	void addNewModel(DynamicMesh* mesh, Texture* texture);


};
