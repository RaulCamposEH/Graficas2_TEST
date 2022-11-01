#ifndef _dxrr
#define _dxrr
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <d3dx10math.h>
#include "TerrenoRR.h"
#include "Camara.h"
#include "SkyDome.h"
#include "Billboard.h"
#include "ModeloRR.h"
#include "XACT3Util.h"
#include "GameModel.h"
#include "Player.h"
#include "Gallina.h"

class DXRR{	

private:
	int ancho;
	int alto;
public:	
	HINSTANCE hInstance;
	HWND hWnd;

#pragma region directx stuff
	D3D_DRIVER_TYPE driverType;
	D3D_FEATURE_LEVEL featureLevel;

	ID3D11Device* d3dDevice;
	ID3D11DeviceContext* d3dContext;
	IDXGISwapChain* swapChain;
	ID3D11RenderTargetView* backBufferTarget;

	ID3D11Texture2D* depthTexture;
	ID3D11DepthStencilView* depthStencilView;

	ID3D11DepthStencilState* depthStencilState;
	ID3D11DepthStencilState* depthStencilDisabledState;

	ID3D11BlendState* alphaBlendState, * commonBlendState;

	XACTINDEX cueIndex;
	CXACT3Util m_XACT3;
#pragma endregion

	int frameBillboard;

#pragma region Scene Stuff

	TerrenoRR* terreno;
	SkyDome* skydome;
	BillboardRR* billboard;
	Camara* camara;

	ModeloRR* model;
	ModeloRR* carrito;
	ModeloRR* barn;
	ModeloRR* gallina;
	ModeloRR* caballo;
	ModeloRR* granero;
	ModeloRR* heno;
	ModeloRR* personaje;
	ModeloRR* tronco;
	ModeloRR* garage;

	Player* Jugador;
	Gallina* chickenOne;
	Gallina* chickenTwo;
	Gallina* chickenThree;
	Item* item;

	ResourceCollection CharacterTextures;
	GameModel* Character;
	ResourceCollection ChickenTextures;
	GameModel* Chicken;
	ResourceCollection FoodTextures;
	GameModel* ChickenFood;	
	ResourceCollection FoodBagTextures;
	GameModel* FoodBag;

	ColArray Colisiones;
#pragma endregion


	float izqder;
	float arriaba;
	float vel;
	bool breakpoint;
	vector2 uv1[32];
	vector2 uv2[32];
	vector2 uv3[32];
	vector2 uv4[32];
	float prueba = 0.0f;

	bool camaraTipo;
	float rotCam;
	
	void addTex(ResourceCollection& col, int number, const wchar_t* texture) {
		col.emplace_back(number, texture);
	}
	void addColisionBox(ColArray& arr, fvec3 origin, fvec3 scale) {
		arr.emplace_back(origin, scale);
	}

    DXRR(HWND hWnd, int Ancho, int Alto)
	{
		breakpoint = false;
		frameBillboard = 0;
		ancho = Ancho;
		alto = Alto;
		driverType = D3D_DRIVER_TYPE_NULL;
		featureLevel = D3D_FEATURE_LEVEL_11_0;
		d3dDevice = 0;
		d3dContext = 0;
		swapChain = 0;
		backBufferTarget = 0;
		IniciaD3D(hWnd);
		izqder = 0;
		arriaba = 0;
		billCargaFuego();

		auto eye = D3DXVECTOR3(0, 80, 6);
		auto target = D3DXVECTOR3(0, 80, 0);
		auto up = D3DXVECTOR3(0, 1, 0);
		camara = new Camara(eye, target, up, Ancho, Alto);

		CharacterTextures.reserve(2);
		addTex(CharacterTextures, 0, L"Assets/personaje/personajeColor.png");
		addTex(CharacterTextures, 1, L"Assets/personaje/personajeSpec.png");

		Character = new GameModel(d3dDevice, d3dContext, "Assets/personaje/personaje.obj", D3DXVECTOR3(60, 0, 30), CharacterTextures);
		Jugador = new Player(Character, camara, fvec3(2.0f, 10.0f, 2.0f));

		ChickenTextures.reserve(2);
		addTex(ChickenTextures, 0, L"Assets/gallina/gallina.png");
		addTex(ChickenTextures, 1, L"Assets/noSpecMap.jpg");

		Chicken = new GameModel(d3dDevice, d3dContext, "Assets/gallina/gallina.obj", D3DXVECTOR3(0, 0, 0), ChickenTextures);
		chickenOne   = new Gallina(Chicken, fvec3(1.0f, 1.0f, 1.0f), 50.0f);
		chickenTwo   = new Gallina(Chicken, fvec3(1.0f, 1.0f, 1.0f), 50.0f);
		chickenThree = new Gallina(Chicken, fvec3(1.0f, 1.0f, 1.0f), 50.0f);
		
		chickenOne->SetPos(fvec3(10.0f, 0.0f, 80.0f));
		chickenTwo->SetPos(fvec3(40.0f, 0.0f, 80.0f));
		chickenThree->SetPos(fvec3(80.0f, 0.0f, 80.0f));

		FoodTextures.reserve(2);
		addTex(FoodTextures, 0, L"Assets/Semillas/SemillasColor.png");
		addTex(FoodTextures, 1, L"Assets/noSpecMap.jpg");

		ChickenFood = new GameModel(d3dDevice, d3dContext, "Assets/comida/comida.obj", D3DXVECTOR3(0, 0, 0), FoodTextures);

		FoodBagTextures.reserve(2);
		addTex(FoodBagTextures, 0, L"Assets/Bolsa/BolsaColor.png");
		addTex(FoodTextures, 1, L"Assets/noSpecMap.jpg");

		FoodBag = new GameModel(d3dDevice, d3dContext, "Assets/Bolsa/Bolsa.obj", D3DXVECTOR3(0, 0, 0), FoodBagTextures);

		item = new Item(ChickenFood, 20.0f);
		item->SetPos(fvec3(80, 0, 0));

		terreno = new TerrenoRR(1200, 1200, d3dDevice, d3dContext);
		skydome = new SkyDome(32, 32, 100.0f, &d3dDevice, &d3dContext, L"SKYD1.png");

		billboard = new BillboardRR(L"Assets/Billboards/fuego-anim.png",L"Assets/Billboards/fuego-anim-normal.png", d3dDevice, d3dContext, 5);
		model = new ModeloRR(d3dDevice, d3dContext, "Assets/Cofre/Cofre.obj", L"Assets/Cofre/Cofre-color.png", L"Assets/Cofre/Cofre-spec.png", 0, 0);
		barn = new ModeloRR(d3dDevice, d3dContext, "Assets/barn/barn.obj", L"Assets/barn/barnColor.png", L"Assets/noSpecMap.jpg", -40, 0);
		garage = new ModeloRR(d3dDevice, d3dContext, "Assets/garage/garage.obj", L"Assets/garage/garageColor.jpg", L"Assets/noSpecMap.jpg", 40, 20);
		granero = new ModeloRR(d3dDevice, d3dContext, "Assets/granero/granero.obj", L"Assets/granero/graneroColor.png", L"Assets/noSpecMap.jpg", 40, 0);
		carrito = new ModeloRR(d3dDevice, d3dContext, "Assets/Cheep.obj", L"Assets/Cheep.jpg", L"Assets/imagen1.jpg", 0, 0);
		gallina = new ModeloRR(d3dDevice, d3dContext, "Assets/gallina/gallina.obj", L"Assets/gallina/gallina.png", L"Assets/noSpecMap.jpg", 10, 0);
		caballo = new ModeloRR(d3dDevice, d3dContext, "Assets/camioneta/camioneta.obj", L"Assets/camioneta/camionetaColor.jpg", L"Assets/camioneta/camionetaSpec.jpg", 20, 0);
		heno = new ModeloRR(d3dDevice, d3dContext, "Assets/heno/heno.obj", L"Assets/heno/henoColor.png", L"Assets/noSpecMap.jpg", 60, 0);

		tronco = new ModeloRR(d3dDevice, d3dContext, "Assets/tronco/tronco.obj", L"Assets/tronco/troncoColor.jpg", L"Assets/tronco/TroncoSpec.jpg", 50, 10);

		//prueba = Character->getX();
		camaraTipo = true;
		rotCam = 0.0f;
	}

	~DXRR()
	{
		LiberaD3D();
		m_XACT3.Terminate();
	}
	
	bool IniciaD3D(HWND hWnd)
	{
		this->hInstance = hInstance;
		this->hWnd = hWnd;
		//obtiene el ancho y alto de la ventana donde se dibuja
		RECT dimensions;
		GetClientRect(hWnd, &dimensions);
		unsigned int width = dimensions.right - dimensions.left;
		unsigned int heigth = dimensions.bottom - dimensions.top;

		//Las formas en como la pc puede ejecutar el DX11, la mas rapida es D3D_DRIVER_TYPE_HARDWARE pero solo se puede usar cuando lo soporte el hardware
		//otra opcion es D3D_DRIVER_TYPE_WARP que emula el DX11 en los equipos que no lo soportan
		//la opcion menos recomendada es D3D_DRIVER_TYPE_SOFTWARE, es la mas lenta y solo es util cuando se libera una version de DX que no sea soportada por hardware
		D3D_DRIVER_TYPE driverTypes[]=
		{
			D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_SOFTWARE
		};
		unsigned int totalDriverTypes = ARRAYSIZE(driverTypes);

		//La version de DX que utilizara, en este caso el DX11
		D3D_FEATURE_LEVEL featureLevels[]=
		{
			D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0
		};
		unsigned int totalFeaturesLevels = ARRAYSIZE(featureLevels);

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = width;
		swapChainDesc.BufferDesc.Height = heigth;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = hWnd;
		swapChainDesc.Windowed = true;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		HRESULT result;
		unsigned int driver = 0, creationFlags = 0;
		for(driver = 0; driver<totalDriverTypes; driver++)
		{
			result = D3D11CreateDeviceAndSwapChain(0, driverTypes[driver], 0,
				creationFlags, featureLevels, totalFeaturesLevels, 
				D3D11_SDK_VERSION, &swapChainDesc, &swapChain,
				&d3dDevice, &featureLevel, &d3dContext);

			if(SUCCEEDED(result))
			{
				driverType = driverTypes[driver];
				break;
			}
		}

		if(FAILED(result))
		{

			//Error al crear el Direct3D device
			return false;
		}
		
		ID3D11Texture2D* backBufferTexture;
		result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferTexture);
		if(FAILED(result))
		{
			//"Error al crear el swapChainBuffer
			return false;
		}

		result = d3dDevice->CreateRenderTargetView(backBufferTexture, 0, &backBufferTarget);
		if(backBufferTexture)
			backBufferTexture->Release();

		if(FAILED(result))
		{
			//Error al crear el renderTargetView
			return false;
		}


		D3D11_VIEWPORT viewport;
		viewport.Width = (FLOAT)width;
		viewport.Height = (FLOAT)heigth;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;

		d3dContext->RSSetViewports(1, &viewport);

		D3D11_TEXTURE2D_DESC depthTexDesc;
		ZeroMemory(&depthTexDesc, sizeof(depthTexDesc));
		depthTexDesc.Width = width;
		depthTexDesc.Height = heigth;
		depthTexDesc.MipLevels = 1;
		depthTexDesc.ArraySize = 1;
		depthTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthTexDesc.SampleDesc.Count = 1;
		depthTexDesc.SampleDesc.Quality = 0;
		depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
		depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthTexDesc.CPUAccessFlags = 0;
		depthTexDesc.MiscFlags = 0;
		
		result = d3dDevice->CreateTexture2D(&depthTexDesc, NULL, &depthTexture);
		if(FAILED(result))
		{
			MessageBox(0, L"Error", L"Error al crear la DepthTexture", MB_OK);
			return false;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV, sizeof(descDSV));
		descDSV.Format = depthTexDesc.Format;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;

		result = d3dDevice->CreateDepthStencilView(depthTexture, &descDSV, &depthStencilView);
		if(FAILED(result))
		{
			MessageBox(0, L"Error", L"Error al crear el depth stencil target view", MB_OK);
			return false;
		}

		d3dContext->OMSetRenderTargets(1, &backBufferTarget, depthStencilView);

		return true;			
		
	}

	void LiberaD3D(void)
	{
		if(depthTexture)
			depthTexture->Release();
		if(depthStencilView)
			depthStencilView->Release();
		if(backBufferTarget)
			backBufferTarget->Release();
		if(swapChain)
			swapChain->Release();
		if(d3dContext)
			d3dContext->Release();
		if(d3dDevice)
			d3dDevice->Release();

		depthTexture = 0;
		depthStencilView = 0;
		d3dDevice = 0;
		d3dContext = 0;
		swapChain = 0;
		backBufferTarget = 0;

		delete terreno;
		delete skydome;
		delete billboard;
		delete camara;
		delete model;
		delete carrito;
		delete barn;
		delete gallina;
		delete caballo;
		delete granero;
		delete heno;
		delete personaje;
		delete tronco;
		delete garage;
		delete Character;

		CharacterTextures.clear();
		ChickenTextures.clear();

	}
	
	void Update(void) {
		if (d3dContext == 0)
			return;

		rotCam += izqder;
		/*float sphere[3] = { 0,0,0 };
		float prevPos[3] = { camara->posCam.x, camara->posCam.z, camara->posCam.z };
		static float angle = 0.0f;
		angle += 0.005;
		if (angle >= 360) angle = 0.0f;*/
		//bool collide = false;

		float clearColor[4] = { 0, 0, 0, 1.0f };
		d3dContext->ClearRenderTargetView(backBufferTarget, clearColor);
		d3dContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		camara->posCam.y = terreno->Superficie(camara->posCam.x, camara->posCam.z) + 17;
		camara->posCam3P.y = terreno->Superficie(camara->posCam3P.x, camara->posCam3P.z) + 17;
		camara->UpdateCam(vel, arriaba, izqder);
		skydome->Update(camara->vista, camara->proyeccion);

		float camPosXZ[2] = { camara->posCam.x, camara->posCam.z };
		//Character->setPos(D3DXVECTOR3(Character->getX() + prueba, terreno->Superficie(Character->getX() + prueba, Character->getZ()), Character->getZ()));
		float x = camara->posCam.x;
		float z = camara->posCam.z;
		float y = camara->posCam.y;

		item->SetAltura(terreno->Superficie(item->getX(), item->getZ()));
		fvec3 pos = fvec3(x, y, z);
		Jugador->Update(pos, Colisiones);
		Jugador->obtenerItem(item);
		Jugador->SetAltura(terreno->Superficie(pos.x, pos.z));

		//a partir de aqui la variable "pos" se manda por referencia y obtiene las posiciones de las gallinas para poder actualizar su altura posteriormente
		chickenOne->Update(Jugador, pos);
		chickenOne->SetAltura(terreno->Superficie(pos.x, pos.z));

		chickenTwo->Update(Jugador, pos);
		chickenTwo->SetAltura(terreno->Superficie(pos.x, pos.z));

		chickenThree->Update(Jugador, pos);
		chickenThree->SetAltura(terreno->Superficie(pos.x, pos.z));

		item->Update();
	}

	void Render(void)
	{
		if (d3dContext == 0)
			return;

		TurnOffDepth();
		skydome->Render(camara->posCam);
		TurnOnDepth();
		terreno->Draw(camara->vista, camara->proyeccion);
		//TurnOnAlphaBlending();
		//billboard->Draw(camara->vista, camara->proyeccion, camara->posCam,
			//-11, -78, 4, 5, uv1, uv2, uv3, uv4, frameBillboard);
		//TurnOffAlphaBlending();
		//model->Draw(camara->vista, camara->proyeccion, terreno->Superficie(0, 0), camara->posCam, 10.0f, 0, 'A', 1, camaraTipo, false);		
		
		garage->Draw(camara->vista, camara->proyeccion, terreno->Superficie(garage->getPosX(), garage->getPosX()), camara->posCam, 1.0f, 0, 'A', 1, camaraTipo, false);
		barn->Draw(camara->vista, camara->proyeccion, terreno->Superficie(barn->getPosX(), barn->getPosZ()), camara->posCam, 1.0f, 0, 'A', 1, camaraTipo, false);
		granero->Draw(camara->vista, camara->proyeccion, terreno->Superficie(granero->getPosX(), granero->getPosX()), camara->posCam, 1.0f, 0, 'A', 1, camaraTipo, false);
		//gallina->Draw(camara->vista, camara->proyeccion, terreno->Superficie(gallina->getPosX(), gallina->getPosZ()), camara->posCam, 1.0f, 0, 'A', 1, camaraTipo, false);
		caballo->Draw(camara->vista, camara->proyeccion, terreno->Superficie(caballo->getPosX(), caballo->getPosX()), camara->posCam, 1.0f, 0, 'A', 1, camaraTipo, false);
		heno->Draw(camara->vista, camara->proyeccion, terreno->Superficie(heno->getPosX(), heno->getPosZ()), camara->posCam, 1.0f, 0, 'A', 1, camaraTipo, false);
		tronco->Draw(camara->vista, camara->proyeccion, terreno->Superficie(tronco->getPosX(), tronco->getPosZ()), camara->posCam, 1.0f, 0, 'A', 1, camaraTipo, false);

		/*float angulo1 = 0;
		float angulo2 = 0;
		float angulo3 = 0;*/

		//giroGallina(angulo1, camara->posCam.z, camara->posCam.x, chickenOne->GetPos().z, chickenOne->GetPos().x);
		//giroGallina(angulo2, camara->posCam.z, camara->posCam.x, chickenTwo->GetPos().z, chickenTwo->GetPos().x);
		//giroGallina(angulo3, camara->posCam.z, camara->posCam.x, chickenThree->GetPos().z, chickenThree->GetPos().x);

		//if (camara->posCam.z < chickenOne->GetPos().z) {           // GIRO DEL PRIMER ZOMBIE
		//	float auxX = chickenOne->GetPos().x - camara->posCam.x;
		//	float auxz = chickenOne->GetPos().z - camara->posCam.z;
		//	angulo1 = atan(auxX / auxz) - 3.14159f;
		//}
		//if (camara->posCam.z >= chickenOne->GetPos().z) {
		//	float auxX = camara->posCam.x - chickenOne->GetPos().x;
		//	float auxz = camara->posCam.z - chickenOne->GetPos().z;
		//	angulo1 = atan(auxX / auxz);
		//}

		Jugador->Draw(camara, 'A', 0, 1.0f, 1.0f);
		
		chickenOne->Draw(camara, 1.0f, 1.0f);
		chickenTwo->Draw(camara,  1.0f, 1.0f);
		chickenThree->Draw(camara, 1.0f, 1.0f);

		item->Draw(camara, 1.0f, 1.0f);

		swapChain->Present( 1, 0 );
	}


	bool isPointInsideSphere(float* point, float* sphere) {
		bool collition = false;

		float distance = sqrt((point[0] - sphere[0]) * (point[0] - sphere[0]) +
			(point[1] - sphere[1]) * (point[1] - sphere[1]));

		if (distance < sphere[2])
			collition = true;
		return collition;
	}

	void TurnOnAlphaBlending()
	{
		float blendFactor[4];
		blendFactor[0] = 0.0f;
		blendFactor[1] = 0.0f;
		blendFactor[2] = 0.0f;
		blendFactor[3] = 0.0f;
		HRESULT result;

		D3D11_BLEND_DESC descABSD;
		ZeroMemory(&descABSD, sizeof(D3D11_BLEND_DESC));
		descABSD.RenderTarget[0].BlendEnable = TRUE;
		descABSD.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		descABSD.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		descABSD.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		descABSD.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		descABSD.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		descABSD.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		descABSD.RenderTarget[0].RenderTargetWriteMask = 0x0f;

		result = d3dDevice->CreateBlendState(&descABSD, &alphaBlendState);
		if(FAILED(result))
		{
			MessageBox(0, L"Error", L"Error al crear el blend state", MB_OK);
			return;
		}

		d3dContext->OMSetBlendState(alphaBlendState, blendFactor, 0xffffffff);
	}

	void TurnOffAlphaBlending()
	{
		D3D11_BLEND_DESC descCBSD;
		ZeroMemory(&descCBSD, sizeof(D3D11_BLEND_DESC));
		descCBSD.RenderTarget[0].BlendEnable = FALSE;
		descCBSD.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		descCBSD.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		descCBSD.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		descCBSD.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		descCBSD.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		descCBSD.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		descCBSD.RenderTarget[0].RenderTargetWriteMask = 0x0f;
		HRESULT result;

		result = d3dDevice->CreateBlendState(&descCBSD, &commonBlendState);
		if(FAILED(result))
		{
			MessageBox(0, L"Error", L"Error al crear el blend state", MB_OK);
			return;
		}

		d3dContext->OMSetBlendState(commonBlendState, NULL, 0xffffffff);
	}

	void TurnOnDepth()
	{
		D3D11_DEPTH_STENCIL_DESC descDSD;
		ZeroMemory(&descDSD, sizeof(descDSD));
		descDSD.DepthEnable = true;
		descDSD.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		descDSD.DepthFunc = D3D11_COMPARISON_LESS;
		descDSD.StencilEnable=true;
		descDSD.StencilReadMask = 0xFF;
		descDSD.StencilWriteMask = 0xFF;
		descDSD.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		descDSD.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		descDSD.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		descDSD.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		descDSD.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		descDSD.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		descDSD.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		descDSD.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		d3dDevice->CreateDepthStencilState(&descDSD, &depthStencilState);
		
		d3dContext->OMSetDepthStencilState(depthStencilState, 1);
	}

	void TurnOffDepth()
	{
		D3D11_DEPTH_STENCIL_DESC descDDSD;
		ZeroMemory(&descDDSD, sizeof(descDDSD));
		descDDSD.DepthEnable = false;
		descDDSD.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		descDDSD.DepthFunc = D3D11_COMPARISON_LESS;
		descDDSD.StencilEnable=true;
		descDDSD.StencilReadMask = 0xFF;
		descDDSD.StencilWriteMask = 0xFF;
		descDDSD.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		descDDSD.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		descDDSD.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		descDDSD.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		descDDSD.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		descDDSD.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		descDDSD.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		descDDSD.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		d3dDevice->CreateDepthStencilState(&descDDSD, &depthStencilDisabledState);
		d3dContext->OMSetDepthStencilState(depthStencilDisabledState, 1);
	}

	void billCargaFuego()
	{
		uv1[0].u = .125;
		uv2[0].u = .125;
		uv3[0].u = 0;
		uv4[0].u = 0;

		uv1[0].v = .25;
		uv2[0].v = 0;
		uv3[0].v = 0;
		uv4[0].v = .25;


		for (int j = 0; j < 8; j++) {
			uv1[j].u = uv1[0].u + (j * .125);
			uv2[j].u = uv2[0].u + (j * .125);
			uv3[j].u = uv3[0].u + (j * .125);
			uv4[j].u = uv4[0].u + (j * .125);

			uv1[j].v = .25;
			uv2[j].v = 0;
			uv3[j].v = 0;
			uv4[j].v = .25;
		}
		for (int j = 0; j < 8; j++) {
			uv1[j + 8].u = uv1[0].u + (j * .125);
			uv2[j + 8].u = uv2[0].u + (j * .125);
			uv3[j + 8].u = uv3[0].u + (j * .125);
			uv4[j + 8].u = uv4[0].u + (j * .125);

			uv1[j + 8].v = .5;
			uv2[j + 8].v = .25;
			uv3[j + 8].v = .25;
			uv4[j + 8].v = .5;
		}

		for (int j = 0; j < 8; j++) {
			uv1[j + 16].u = uv1[0].u + (j * .125);
			uv2[j + 16].u = uv2[0].u + (j * .125);
			uv3[j + 16].u = uv3[0].u + (j * .125);
			uv4[j + 16].u = uv4[0].u + (j * .125);

			uv1[j + 16].v = .75;
			uv2[j + 16].v = .5;
			uv3[j + 16].v = .5;
			uv4[j + 16].v = .75;
		}

		for (int j = 0; j < 8; j++) {
			uv1[j + 24].u = uv1[0].u + (j * .125);
			uv2[j + 24].u = uv2[0].u + (j * .125);
			uv3[j + 24].u = uv3[0].u + (j * .125);
			uv4[j + 24].u = uv4[0].u + (j * .125);

			uv1[j + 24].v = 1;
			uv2[j + 24].v = .75;
			uv3[j + 24].v = .75;
			uv4[j + 24].v = 1;
		}
	}

};
#endif