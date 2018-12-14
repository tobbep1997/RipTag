#pragma once
#include "2D Engine/Quad/Quad.h"

class LoadingScreen
{
private:
	Quad* m_loadingText = nullptr;  
	float m_alphaValue = 0.0f; 
	float m_frequency = 3.0f; 
public:
	LoadingScreen(); 
	~LoadingScreen(); 
	void Init();

	void removeGUI(std::vector<Quad*>& GUICollection); 

	void draw(); 
	void update(); 
};