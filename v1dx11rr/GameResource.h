#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <DxErr.h>
#include <D3Dcompiler.h>
#include <d3dx10math.h>
#include <conio.h>
#include <vector>
#include <iostream>


struct GameResource
{
	GameResource() {

	}

	GameResource(int _pos, const wchar_t* _path):
		path(_path), bufferpos(_pos)
	{
	}

	GameResource(GameResource* origin) {
		this->path = origin->path;
		this->Resource = origin->Resource;
		this->bufferpos = origin->bufferpos;
	}

	ID3D11ShaderResourceView* Resource;
	const wchar_t *path;
	int bufferpos;
};

