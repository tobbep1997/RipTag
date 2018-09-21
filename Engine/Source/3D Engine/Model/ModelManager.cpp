#include "ModelManager.h"



ModelManager::ModelManager()
{
	

}


ModelManager::~ModelManager()
{
	/*for (int i = 0; i < dynamicMesh.size(); i++)
	{
		delete dynamicMesh[i];
	}
	for (int i = 0; i < staticMesh.size(); i++)
	{
		delete staticMesh[i];
	}*/
}

void ModelManager::DrawMeshes()
{
	for (int i = 0; i < staticMesh.size(); i++)
	{
		//FICA SÅ ATT DENNA INTE BEHÖVS -> FPS = 0
		//staticMesh[i]->setTexture(staticMesh[i]->assetName);
		staticMesh[i]->Draw();

	}
	for (int i = 0; i < dynamicMesh.size(); i++)
	{
		dynamicMesh[i]->DrawAnimated();
	}
}

void ModelManager::bindTextures(const std::string& assetName)
{
	Texture tempTex;
	std::wstring asset;
	asset.assign(assetName.begin(), assetName.end());
	tempTex.Load(asset.c_str());
	tempTex.Bind(1);
	// std::string -> std::wstring
	//std::string s("string");
	//std::wstring ws;
	//ws.assign(s.begin(), s.end());
	meshtextures.push_back(tempTex);
}

void ModelManager::addStaticMesh(const std::string & assetFilePath)
{
	
	//int i = 0; 
	//Model * tempModel = new Model(ObjectType::Static, 
	//	assetFilePath
	//	);
	////STRING TO WSTRING 
	//
	////tempModel->setTexture(tempModel->assetName);
	//bindTextures(tempModel->assetName);
	//staticMesh.push_back(tempModel);
}

void ModelManager::addDynamicMesh(const std::string & assetFilePath)
{
	///*std::string tempString = "../Assets/";
	//tempString.append(assetFilePath + "/" + assetFilePath + ".bin");*/
	//int i = 0;
	//Model * tempModel = new Model(ObjectType::Dynamic,
	//	assetFilePath
	//	);
	//dynamicMesh.push_back(tempModel);
}





