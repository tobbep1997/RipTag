#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <debugapi.h>
#include <comdef.h>
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
/*
Copyright (Joa)king Trossvik 1856
*/
namespace ShaderCreator //Maybe subject to change
{
	inline void CreateVertexShader(ID3D11Device* device, ID3D11VertexShader*& vertexShader, const LPCWSTR fileName, const LPCSTR entryPoint, D3D11_INPUT_ELEMENT_DESC inputDesc[], int arraySize, ID3D11InputLayout*& inputLayout)
	{
		HRESULT hr;
		ID3DBlob* pVS = nullptr;
		//ID3DBlob* error = nullptr;
		//ID3DBlob* errorBlob = nullptr;
		hr = D3DCompileFromFile(
			fileName,		// filename
			nullptr,		// optional macros
			nullptr,		// optional include files
			entryPoint,		// entry point
			"vs_5_0",		// shader model (target)
			0,				// shader compile options
			0,				// effect compile options
			&pVS,			// double pointer to ID3DBlob		
			nullptr		// pointer for Error Blob messages.
		);

		//This is for debugging, if we miss a file or whatever the hr will tell us that
		if (FAILED(hr))
		{
			_com_error err(hr);
			OutputDebugString(err.ErrorMessage());
			OutputDebugStringA((char*)" :Vertex Shader:");

			if (pVS)
			{
				pVS->Release();
			}

		}
		//Yes the program will crash if we hit the debugging
		//But there is no reasen to keep the program going, it will only mess with the shaders
		//So this is to shorten DebugTime
		device->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &vertexShader);

		if (FAILED(hr = device->CreateInputLayout(inputDesc, arraySize, pVS->GetBufferPointer(), pVS->GetBufferSize(), &inputLayout)))
		{

			_com_error err(hr);
			OutputDebugString(err.ErrorMessage());
			OutputDebugStringA((char*)" :Vertex Shader:");
			pVS->Release();
		}

		pVS->Release();
	}

	inline void CreateVertexShader(ID3D11Device* device, ID3D11VertexShader*& vertexShader, const LPCWSTR fileName, const LPCSTR entryPoint)
	{
		HRESULT hr;
		ID3DBlob* pVS = nullptr;
		//ID3DBlob* error = nullptr;
		//ID3DBlob* errorBlob = nullptr;
		hr = D3DCompileFromFile(
			fileName,		// filename
			nullptr,		// optional macros
			nullptr,		// optional include files
			entryPoint,		// entry point
			"vs_5_0",		// shader model (target)
			0,				// shader compile options
			0,				// effect compile options
			&pVS,			// double pointer to ID3DBlob		
			nullptr		// pointer for Error Blob messages.
		);

		//This is for debugging, if we miss a file or whatever the hr will tell us that
		if (FAILED(hr))
		{
			_com_error err(hr);
			OutputDebugString(err.ErrorMessage());
			OutputDebugStringA((char*)" :Vertex Shader:");

			if (pVS)
			{
				pVS->Release();
			}

		}
		//Yes the program will crash if we hit the debugging
		//But there is no reasen to keep the program going, it will only mess with the shaders
		//So this is to shorten DebugTime
		device->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &vertexShader);

		pVS->Release();
	}

	inline void CreateDomainShader(ID3D11Device* device, ID3D11DomainShader*& domainShader, const LPCWSTR fileName, const LPCSTR entryPoint = "main")
	{
		HRESULT hr = 0;
		ID3DBlob* pDS = nullptr;
		ID3DBlob* error = nullptr;
		//ID3DBlob* errorBlob = nullptr;
		D3DCompileFromFile(
			fileName,		// filename
			nullptr,		// optional macros
			nullptr,		// optional include files
			entryPoint,		// entry point
			"ds_5_0",		// shader model (target)
			0,				// shader compile options
			0,				// effect compile options
			&pDS,			// double pointer to ID3DBlob		
			&error		// pointer for Error Blob messages.
		);

		if (FAILED(hr))
		{
			_com_error err(hr);
			OutputDebugString(err.ErrorMessage());
			OutputDebugStringA((char*)" :Domain Shader:");

			if (pDS)
			{
				pDS->Release();
			}

		}

		device->CreateDomainShader(pDS->GetBufferPointer(), pDS->GetBufferSize(), nullptr, &domainShader);

		pDS->Release();
	}

	inline void CreateHullShader(ID3D11Device* device, ID3D11HullShader*& hullShader, const LPCWSTR fileName, const LPCSTR entryPoint = "main")
	{
		HRESULT hr;
		ID3DBlob* pHS = nullptr;
		//ID3DBlob* error = nullptr;
		hr = D3DCompileFromFile(
			fileName,		// filename
			nullptr,		// optional macros
			nullptr,		// optional include files
			entryPoint,		// entry point
			"hs_5_0",		// shader model (target)
			0,				// shader compile options
			0,				// effect compile options
			&pHS,			// double pointer to ID3DBlob		
			nullptr		// pointer for Error Blob messages.
		);

		if (FAILED(hr))
		{
			_com_error err(hr);
			OutputDebugString(err.ErrorMessage());
			OutputDebugStringA((char*)" :Hull Shader:");

			if (pHS)
			{
				pHS->Release();
			}

		}

		device->CreateHullShader(pHS->GetBufferPointer(), pHS->GetBufferSize(), nullptr, &hullShader);

		pHS->Release();
	}

	inline bool CreateGeometryShader(ID3D11Device* device, ID3D11GeometryShader*& geometryShader, const LPCWSTR fileName, const LPCSTR entryPoint = "main")
	{
		HRESULT hr;
		ID3DBlob* pGS = nullptr;
		//ID3DBlob* error = nullptr;
		hr = D3DCompileFromFile(
			fileName,		// filename
			nullptr,		// optional macros
			nullptr,		// optional include files
			entryPoint,		// entry point
			"gs_5_0",		// shader model (target)
			0,				// shader compile options
			0,				// effect compile options
			&pGS,			// double pointer to ID3DBlob		
			nullptr		// pointer for Error Blob messages.
		);

		if (FAILED(hr))
		{
			_com_error err(hr);
			OutputDebugString(err.ErrorMessage());
			OutputDebugStringA((char*)" :Geometry Shader:");

			if (pGS)
			{
				pGS->Release();
			}
			return false;

		}
		else
		{
			hr = device->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(), nullptr, &geometryShader);

			pGS->Release();

			return true;
		}
	}

	inline bool CreatePixelShader(ID3D11Device* device, ID3D11PixelShader*& pixelShader, const LPCWSTR fileName, const LPCSTR entryPoint = "main")
	{
		HRESULT hr;
		ID3DBlob* pPS = nullptr;
		//ID3DBlob* error = nullptr;
		hr = D3DCompileFromFile(
			fileName,		// filename
			nullptr,		// optional macros
			nullptr,		// optional include files
			entryPoint,		// entry point
			"ps_5_0",		// shader model (target)
			0,				// shader compile options
			0,				// effect compile options
			&pPS,			// double pointer to ID3DBlob		
			nullptr		// pointer for Error Blob messages.
		);

		if (FAILED(hr))
		{
			_com_error err(hr);
			OutputDebugString(err.ErrorMessage());
			OutputDebugStringA((char*)" :Pixel Shader:");

			if (pPS)
			{
				pPS->Release();
			}
			return false;
		}
		else
		{

			device->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &pixelShader);

			pPS->Release();
			return true;
		}
	}

	inline void CreateComputeShader(ID3D11Device* device, ID3D11ComputeShader*& computeShader, const LPCWSTR fileName, const LPCSTR entryPoint = "main")
	{
		HRESULT hr;
		ID3DBlob* pCS = nullptr;
		//ID3DBlob* error = nullptr;
		hr = D3DCompileFromFile(
			fileName,		// filename
			nullptr,		// optional macros
			nullptr,		// optional include files
			entryPoint,		// entry point
			"cs_5_0",		// shader model (target)
			0,				// shader compile options
			0,				// effect compile options
			&pCS,			// double pointer to ID3DBlob		
			nullptr		// pointer for Error Blob messages.
		);

		if (FAILED(hr))
		{
			_com_error err(hr);
			OutputDebugString(err.ErrorMessage());
			OutputDebugStringA((char*)" :Compute Shader:");

			if (pCS)
			{
				pCS->Release();
			}

		}

		device->CreateComputeShader(pCS->GetBufferPointer(), pCS->GetBufferSize(), nullptr, &computeShader);

		pCS->Release();
	}
}
