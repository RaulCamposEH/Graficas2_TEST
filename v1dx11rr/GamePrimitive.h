#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <DxErr.h>
#include <D3Dcompiler.h>
#include <d3dx10math.h>
#include "GameResource.h"
#include "ShaderResource.h"
#include "LoadModel.h"
#include <vector>
#include <xnamath.h>
#include <stdlib.h>
#include "Camara.h"
#include "ColBox.h"
#include "Gallina.h"
#include "Player.h"

using namespace std;

class Primitive {
public:
	D3DXVECTOR3 mPosicion;
	float mRadio;

	ID3D11Device* mDevice;
	ID3D11DeviceContext* mContext;

	Shader_info mVsinfo;
	Shader_info mPsinfo;
	ShaderClass* Shader;

	char* mModel_path;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	ID3D11SamplerState* colorMapSampler;

	ID3D11Buffer* viewCB;
	ID3D11Buffer* projCB;
	ID3D11Buffer* worldCB;
	D3DXMATRIX viewMatrix;
	D3DXMATRIX projMatrix;

	CObjParser ObjParser;
	float rotaY;

	ID3D11Buffer* mColorCB;
	XMFLOAT4 mColor;

	Primitive(ID3D11Device* dev, ID3D11DeviceContext* cont, char* path, D3DXVECTOR3 Posicion, float radio, XMFLOAT4 color)
		: mDevice(dev), mContext(cont), mModel_path(path), mColor(color)
	{
		this->mPosicion = Posicion;
		this->mRadio = radio;
		this->Init();
	}

	~Primitive() {
		Unload();
	}

	void Update(Gallina* gallina[], Player* Jugador) {
		for (int i = 0; i < 3; i++) {
			bool col = gallina[i]->GetColBox()->CheckSphereColission(mPosicion, mRadio);
			if (col) {
				bool alreadySaved = gallina[i]->GetSavedState();
				if (!alreadySaved)
				{
					gallina[i]->SetSaved();
					Jugador->SumPoint();
				}
			}
		}

	}

	void Draw(Camara* camara, float scale) {

		unsigned int stride = sizeof(VertexObj);
		unsigned int offset = 0;


		mContext->IASetInputLayout(Shader->inputLayout);
		mContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//Establece el vertex y pixel shader que utilizara
		mContext->VSSetShader(Shader->_VShader, 0, 0);
		mContext->PSSetShader(Shader->_PShader, 0, 0);

		D3DXMATRIX translacionRotCam;
		D3DXMatrixTranslation(&translacionRotCam, 0.0, 0.0, 0.0);
		D3DXMATRIX rotationMat;
		D3DXMatrixRotationYawPitchRoll(&rotationMat, 0.0f, 0.0f, 0.0f);
		//(rotaY*(D3DX_PI/180))
		D3DXMatrixRotationY(&rotationMat, rotaY);

		viewMatrix *= rotationMat;

		D3DXMATRIX translationMat;
		D3DXMatrixTranslation(&translationMat, mPosicion.x, mPosicion.y, mPosicion.z);

		D3DXMATRIX scaleMat;
		D3DXMatrixScaling(&scaleMat, scale, scale * 1.5, scale);

		D3DXMATRIX worldMat = rotationMat * scaleMat * translationMat;

		worldMat = scaleMat * rotationMat * translationMat;

		D3DXMatrixTranspose(&worldMat, &worldMat);
		//actualiza los buffers del shader
		mContext->UpdateSubresource(worldCB, 0, 0, &worldMat, 0, 0);
		mContext->UpdateSubresource(viewCB, 0, 0, &camara->vista, 0, 0);
		mContext->UpdateSubresource(projCB, 0, 0, &camara->proyeccion, 0, 0);
		mContext->UpdateSubresource(mColorCB, 0, 0, &mColor, 0, 0);

		//mContext->UpdateSubresource(cameraPosCB, 0, 0, &camPos, 0, 0);
		//le pasa al shader los buffers
		mContext->VSSetConstantBuffers(0, 1, &worldCB);
		mContext->VSSetConstantBuffers(1, 1, &viewCB);
		mContext->VSSetConstantBuffers(2, 1, &projCB);
		mContext->PSSetConstantBuffers(0, 1, &mColorCB);


		mContext->Draw(ObjParser.m_nVertexCount, 0);
	}

	float getX() { return this->mPosicion.x; }
	float getZ() { return this->mPosicion.z; }
	fvec3 getPos() { return this->mPosicion; }

	void setPos(fvec3 pos) { this->mPosicion = pos; }
	void setYRot(float y) { rotaY = y; }
	void setAltura(float altura) { mPosicion.y = altura; }


	bool Init() {
		Shader = new ShaderClass(L"Primitive.fx", mDevice, mContext);
		ID3DBlob* vsBuffer = 0;
		ID3DBlob* psBuffer = 0;

		D3D11_INPUT_ELEMENT_DESC solidColorLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		unsigned int layoutSize = ARRAYSIZE(solidColorLayout);

		Shader->LoadShaders(solidColorLayout, layoutSize);

		if (!LoadModel()) return false;
	}

	void Unload() {

		if (Shader)
			Shader->Release();
		if (colorMapSampler)
			colorMapSampler->Release();
		if (vertexBuffer)
			vertexBuffer->Release();
		if (viewCB)
			viewCB->Release();
		if (projCB)
			projCB->Release();
		if (worldCB)
			worldCB->Release();
		/*if (cameraPosCB)
			cameraPosCB->Release();*/

		colorMapSampler = 0;
		vertexBuffer = 0;
		indexBuffer = 0;
		viewCB = 0;
		projCB = 0;
		worldCB = 0;
		//cameraPosCB = 0;
	}

private:

	bool LoadModel() {
		HRESULT d3dResult;
		ObjParser.LoadFile(mModel_path);

		D3D11_BUFFER_DESC vertexDesc;
		ZeroMemory(&vertexDesc, sizeof(vertexDesc));
		vertexDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexDesc.ByteWidth = sizeof(VertexObj) * ObjParser.m_nVertexCount;

		D3D11_SUBRESOURCE_DATA resourceData;
		ZeroMemory(&resourceData, sizeof(resourceData));
		resourceData.pSysMem = ObjParser.m_pVertex;

		d3dResult = mDevice->CreateBuffer(&vertexDesc, &resourceData, &vertexBuffer);
		if (FAILED(d3dResult))
		{
			MessageBox(0, L"Error", L"Error al crear vertex buffer", MB_OK);
			return false;
		}

		//if (!CreateShaderResourceView()) return false;
		//if (!CreateSampler()) return false;
		if (!CreateBuffers()) return false;

		//quizas ocupe el ojo el target y el up de la camara??
		D3DXVECTOR3 eye = D3DXVECTOR3(0.0f, 100.0f, 200.0f);
		D3DXVECTOR3 target = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		D3DXMatrixLookAtLH(&viewMatrix, &eye, &target, &up);
		D3DXMatrixPerspectiveFovLH(&projMatrix, D3DX_PI / 4.0, 16 / 9, 0.0001f, 2000.0f);
		D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
		D3DXMatrixTranspose(&projMatrix, &projMatrix);

		return true;
	}

	/*bool CreateShaderResourceView() {
		for (auto& texture : mTextureCollection)
		{
			ID3D11ShaderResourceView* resource;
			if (FAILED(D3DX11CreateShaderResourceViewFromFile(mDevice, texture.path, 0, 0, &resource, 0))) return false;
			texture.Resource = resource;
		}
		return true;
	}

	bool CreateSampler() {
		D3D11_SAMPLER_DESC colorMapDesc;
		ZeroMemory(&colorMapDesc, sizeof(colorMapDesc));
		colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		colorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		colorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;
		//con la estructura de descripion creamos el sampler
		if (FAILED(mDevice->CreateSamplerState(&colorMapDesc, &colorMapSampler))) {
			return false;
		}
		return true;
	}*/

	bool CreateBuffers() {
		//creamos los buffers para el shader para poder pasarle las matrices
		D3D11_BUFFER_DESC constDesc;
		ZeroMemory(&constDesc, sizeof(constDesc));
		constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constDesc.ByteWidth = sizeof(D3DXMATRIX);
		constDesc.Usage = D3D11_USAGE_DEFAULT;

		if (FAILED(mDevice->CreateBuffer(&constDesc, 0, &viewCB))) return false;
		if (FAILED(mDevice->CreateBuffer(&constDesc, 0, &projCB))) return false;
		if (FAILED(mDevice->CreateBuffer(&constDesc, 0, &worldCB))) return false;

		constDesc.ByteWidth = sizeof(XMFLOAT4);

		if (FAILED(mDevice->CreateBuffer(&constDesc, 0, &mColorCB))) return false;
		return true;
	}

};