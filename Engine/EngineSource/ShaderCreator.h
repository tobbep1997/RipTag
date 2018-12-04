#pragma once
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <debugapi.h>
#include <comdef.h>
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
/*
Copyright (Joa)king Trossvik 1856
*/
#ifdef _DEPOLY
	#define COMPILE_SETTINGS D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_PARTIAL_PRECISION | D3DCOMPILE_SKIP_VALIDATION
#else
	#define COMPILE_SETTINGS D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_PARTIAL_PRECISION
#endif
#include <iostream>
namespace ShaderCreator //Maybe subject to change
{
	inline HRESULT CreateVertexShader(ID3D11Device* device, ID3D11VertexShader*& vertexShader, const LPCWSTR fileName, const LPCSTR entryPoint, D3D11_INPUT_ELEMENT_DESC inputDesc[], int arraySize, ID3D11InputLayout*& inputLayout)
	{
		HRESULT hr;
		HRESULT shaderError;
		ID3DBlob* pVS = nullptr;
		ID3DBlob * errorBlob = nullptr;	
		shaderError = D3DCompileFromFile(
			fileName,		// filename
			nullptr,		// optional macros
			D3D_COMPILE_STANDARD_FILE_INCLUDE,		// optional include files
			entryPoint,		// entry point
			"vs_5_0",		// shader model (target)
			COMPILE_SETTINGS,				// shader compile options
			0,				// effect compile options
			&pVS,			// double pointer to ID3DBlob		
			&errorBlob		// pointer for Error Blob messages.
		);

		//This is for debugging, if we miss a file or whatever the hr will tell us that
		if (FAILED(shaderError))
		{
			_com_error err(shaderError);
			OutputDebugString(err.ErrorMessage());
			OutputDebugStringA((char*)" :Vertex Shader:");

			std::cout << ((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();


			if (pVS)
			{
				pVS->Release();
			}
			return shaderError;

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
		return shaderError;
	}

	inline HRESULT CreateVertexShader(ID3D11Device* device, ID3D11VertexShader*& vertexShader, const LPCWSTR fileName, const LPCSTR entryPoint)
	{
		HRESULT hr;
		ID3DBlob* pVS = nullptr;
		ID3DBlob * errorBlob = nullptr;
		hr = D3DCompileFromFile(
			fileName,		// filename
			nullptr,		// optional macros
			D3D_COMPILE_STANDARD_FILE_INCLUDE,		// optional include files
			entryPoint,		// entry point
			"vs_5_0",		// shader model (target)
			COMPILE_SETTINGS,				// shader compile options
			0,				// effect compile options
			&pVS,			// double pointer to ID3DBlob		
			&errorBlob		// pointer for Error Blob messages.
		);

		//This is for debugging, if we miss a file or whatever the hr will tell us that
		if (FAILED(hr))
		{
			_com_error err(hr);
			OutputDebugString(err.ErrorMessage());
			OutputDebugStringA((char*)" :Vertex Shader:");

			std::cout << ((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();

			if (pVS)
			{
				pVS->Release();
			}
			return hr;
		}
		//Yes the program will crash if we hit the debugging
		//But there is no reasen to keep the program going, it will only mess with the shaders
		//So this is to shorten DebugTime
		device->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &vertexShader);

		pVS->Release();
		return hr;
	}

	inline HRESULT CreateDomainShader(ID3D11Device* device, ID3D11DomainShader*& domainShader, const LPCWSTR fileName, const LPCSTR entryPoint = "main")
	{
		HRESULT hr = 0;
		ID3DBlob* pDS = nullptr;
		ID3DBlob * errorBlob = nullptr;

		D3DCompileFromFile(
			fileName,		// filename
			nullptr,		// optional macros
			D3D_COMPILE_STANDARD_FILE_INCLUDE,		// optional include files
			entryPoint,		// entry point
			"ds_5_0",		// shader model (target)
			COMPILE_SETTINGS,				// shader compile options
			0,				// effect compile options
			&pDS,			// double pointer to ID3DBlob		
			&errorBlob		// pointer for Error Blob messages.
		);

		if (FAILED(hr))
		{
			_com_error err(hr);
			OutputDebugString(err.ErrorMessage());
			OutputDebugStringA((char*)" :Domain Shader:");

			std::cout << ((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();

			if (pDS)
			{
				pDS->Release();
			}
			return hr;

		}

		device->CreateDomainShader(pDS->GetBufferPointer(), pDS->GetBufferSize(), nullptr, &domainShader);

		pDS->Release();
		return hr;
	}

	inline HRESULT CreateHullShader(ID3D11Device* device, ID3D11HullShader*& hullShader, const LPCWSTR fileName, const LPCSTR entryPoint = "main")
	{
		HRESULT hr;
		ID3DBlob* pHS = nullptr;
		ID3DBlob * errorBlob = nullptr;

		hr = D3DCompileFromFile(
			fileName,		// filename
			nullptr,		// optional macros
			D3D_COMPILE_STANDARD_FILE_INCLUDE,		// optional include files
			entryPoint,		// entry point
			"hs_5_0",		// shader model (target)
			COMPILE_SETTINGS,				// shader compile options
			0,				// effect compile options
			&pHS,			// double pointer to ID3DBlob		
			&errorBlob		// pointer for Error Blob messages.
		);

		if (FAILED(hr))
		{
			_com_error err(hr);
			OutputDebugString(err.ErrorMessage());
			OutputDebugStringA((char*)" :Hull Shader:");

			std::cout << ((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();

			if (pHS)
			{
				pHS->Release();
			}
			return hr;

		}

		device->CreateHullShader(pHS->GetBufferPointer(), pHS->GetBufferSize(), nullptr, &hullShader);

		pHS->Release();
		return hr;
	}

	inline HRESULT CreateGeometryShader(ID3D11Device* device, ID3D11GeometryShader*& geometryShader, const LPCWSTR fileName, const LPCSTR entryPoint = "main")
	{
		HRESULT hr;
		ID3DBlob* pGS = nullptr;
		ID3DBlob * errorBlob = nullptr;
		hr = D3DCompileFromFile(
			fileName,		// filename
			nullptr,		// optional macros
			D3D_COMPILE_STANDARD_FILE_INCLUDE,		// optional include files
			entryPoint,		// entry point
			"gs_5_0",		// shader model (target)
			COMPILE_SETTINGS,				// shader compile options
			0,				// effect compile options
			&pGS,			// double pointer to ID3DBlob		
			&errorBlob		// pointer for Error Blob messages.
		);

		if (FAILED(hr))
		{
			_com_error err(hr);
			OutputDebugString(err.ErrorMessage());
			OutputDebugStringA((char*)" :Geometry Shader:");

			std::cout << ((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();

			if (pGS)
			{
				pGS->Release();
			}
			return hr;


		}
	
		device->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(), nullptr, &geometryShader);

		pGS->Release();
		return hr;
	}

	inline HRESULT CreatePixelShader(ID3D11Device* device, ID3D11PixelShader*& pixelShader, const LPCWSTR fileName, const LPCSTR entryPoint = "main")
	{
		HRESULT hr;
		ID3DBlob* pPS = nullptr;
		ID3DBlob * errorBlob = nullptr;
		hr = D3DCompileFromFile(
			fileName,		// filename
			nullptr,		// optional macros
			D3D_COMPILE_STANDARD_FILE_INCLUDE,		// optional include files
			entryPoint,		// entry point
			"ps_5_0",		// shader model (target)
			COMPILE_SETTINGS,				// shader compile options
			0,				// effect compile options
			&pPS,			// double pointer to ID3DBlob		
			&errorBlob		// pointer for Error Blob messages.
		);

		if (FAILED(hr))
		{
			_com_error err(hr);
			OutputDebugString(err.ErrorMessage());
			OutputDebugStringA((char*)" :Pixel Shader:");

			std::cout << ((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();

			if (pPS)
			{
				pPS->Release();
			}

			return hr;

		}
	

		device->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &pixelShader);

		pPS->Release();
	
		return hr;

	}

	inline HRESULT CreateComputeShader(ID3D11Device* device, ID3D11ComputeShader*& computeShader, const LPCWSTR fileName, const LPCSTR entryPoint = "main")
	{
		HRESULT hr;
		ID3DBlob* pCS = nullptr;
		ID3DBlob * errorBlob = nullptr;

		hr = D3DCompileFromFile(
			fileName,		// filename
			nullptr,		// optional macros
			D3D_COMPILE_STANDARD_FILE_INCLUDE,		// optional include files
			entryPoint,		// entry point
			"cs_5_0",		// shader model (target)
			COMPILE_SETTINGS,				// shader compile options
			0,				// effect compile options
			&pCS,			// double pointer to ID3DBlob		
			&errorBlob		// pointer for Error Blob messages.
		);

		if (FAILED(hr))
		{
			_com_error err(hr);
			OutputDebugString(err.ErrorMessage());
			OutputDebugStringA((char*)" :Compute Shader:");

			std::cout << ((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();

			if (pCS)
			{
				pCS->Release();
			}
			return hr;

		}

		device->CreateComputeShader(pCS->GetBufferPointer(), pCS->GetBufferSize(), nullptr, &computeShader);

		pCS->Release();
		return hr;
	}
}
