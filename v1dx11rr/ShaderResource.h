#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <DxErr.h>
#include <D3Dcompiler.h>
#include <d3dx10math.h>

struct Shader_info
{
	const wchar_t* path;
	const char* entry;
	const char* version;
};

class ShaderClass {
public:
	ID3D11VertexShader* _VShader;
	ID3D11PixelShader* _PShader;

	ID3D11InputLayout* inputLayout;
private:
	
	Shader_info _vsInfo;
	Shader_info _psInfo;

	ID3D11Device* _device;
	ID3D11DeviceContext* _context;

	bool CompileShader(Shader_info info, ID3DBlob** buffer) {
		DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

		ID3DBlob* errorBuffer = 0;
		HRESULT result;

		result = D3DX11CompileFromFile(info.path, 0, 0, info.entry, info.version, shaderFlags,
			0, 0, buffer, &errorBuffer, 0);
		if (FAILED(result))
		{
			if (errorBuffer != 0)
			{
				OutputDebugStringA((char*)errorBuffer->GetBufferPointer());
				errorBuffer->Release();
			}
			return false;
		}

		if (errorBuffer != 0)
			errorBuffer->Release();
		return true;
	}

	bool LoadShaders(D3D11_INPUT_ELEMENT_DESC layout[], int layout_size) {
		HRESULT result;
		ID3DBlob* vsBuffer = 0;

		bool compileResult = CompileShader(_vsInfo, &vsBuffer);
		//en caso de no poder cargarse ahi muere la cosa
		if (compileResult == false) return false;
		
		result = _device->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), 0, &_VShader);
		//en caso de falla sale
		if (FAILED(result))
		{
			if (vsBuffer) vsBuffer->Release();
			return false;
		}

		result = _device->CreateInputLayout(layout, layout_size,
			vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), &inputLayout);

		vsBuffer->Release();

		if (FAILED(result))
		{
			return false;
		}

		ID3DBlob* psBuffer = 0;

		bool compileResult = CompileShader(_psInfo, &psBuffer);
		//en caso de no poder cargarse ahi muere la cosa
		if (compileResult == false) return false;

		result = _device->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), 0, &_PShader);
		//en caso de falla sale
		if (FAILED(result))
		{
			if (psBuffer) psBuffer->Release();
			return false;
		}
		return true;
	}

	void draw() {

	}
};