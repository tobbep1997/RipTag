#include "MeshManager.h"



MeshManager::MeshManager()
{
}


MeshManager::~MeshManager()
{
	for (int i = 0; i < m_DynamicMesh.size(); i++)
	{
		delete m_DynamicMesh[i];
	}
	for (int i = 0; i < m_StaticMesh.size(); i++)
	{
		delete m_StaticMesh[i];
	}
}

bool MeshManager::loadDynamicMesh(const std::string & meshName)
{
	std::string tempString = "../Assets/";
	tempString.append(meshName /*+ "FOLDER/" + meshName*/ + ".bin");

	DynamicMesh* tempMesh = new DynamicMesh();

	tempMesh = new DynamicMesh();
	tempMesh->LoadMesh(tempString);
	/*SetModel(m_StaticMeshPointer);
	SetVertexShader(L"../Engine/Source/Shader/VertexShader.hlsl");
	SetPixelShader(L"../Engine/Source/Shader/PixelShader.hlsl");
	assetName = assetFilePath;*/
	m_DynamicMesh.push_back(tempMesh);
	
	return true;
}

bool MeshManager::loadStaticMesh(const std::string & meshName)
{
	std::string tempString = "../Assets/";
	tempString.append(meshName + "FOLDER/" + meshName + ".bin");

	StaticMesh* tempMesh = new StaticMesh();

	tempMesh = new StaticMesh();
	tempMesh->LoadMesh(tempString);
	/*SetModel(m_StaticMeshPointer);
	SetVertexShader(L"../Engine/Source/Shader/VertexShader.hlsl");
	SetPixelShader(L"../Engine/Source/Shader/PixelShader.hlsl");
	assetName = assetFilePath;*/
	m_StaticMesh.push_back(tempMesh);

	return true;
}

DynamicMesh * MeshManager::getDynamicMesh(int which)
{
	return m_DynamicMesh[which];
}

StaticMesh * MeshManager::getStaticMesh(int which)
{
	return m_StaticMesh[which];
}
