#pragma once
#include "../../Shader/ShaderManager.h"
class ForwardRender
{
private:
	ID3D11InputLayout * m_inputLayout;
	Shaders::ShaderManager m_sm;
	ID3D11VertexShader * m_vertexShader;
	ID3D11PixelShader * m_pixelShader;
public:
	ForwardRender();
	~ForwardRender();

	
	void GeometryPass();
	void Flush();
	void Present();

	void Release();
private:
	
};

