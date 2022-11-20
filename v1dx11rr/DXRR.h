#ifndef _dxrr
#define _dxrr
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <d3dx10math.h>
#include <string>

#include "TerrenoRR.h"
#include "Camara.h"
#include "SkyDome.h"
#include "Billboard.h"
#include "ModeloRR.h"
#include "XACT3Util.h"
#include "GUI.h"
#include "Agua.h"
#include "Text.h"

#include "GameModel.h"
#include "Player.h"
#include "Gallina.h"
#include "Trampa.h"
#include "GamePrimitive.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"

#include "Saver.h"

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

	AguaRR* Aguita;

	Player* Jugador;
	Gallina* chickenOne;
	Gallina* chickenTwo;
	Gallina* chickenThree;
	Item* item;
	Trampa* trap;
	Primitive* WinTarget;

	ResourceCollection CharacterTextures;
	GameModel* Character;

	ResourceCollection ChickenTextures;
	GameModel* Chicken;

	ResourceCollection FoodTextures;
	GameModel* ChickenFood;	

	ResourceCollection FoodBagTextures;
	GameModel* FoodBag;

	ResourceCollection SiloTextures;
	GameModel* Silo;

	ResourceCollection GraneroTextures;
	GameModel* Granero;

	ResourceCollection TrampaTextures;
	GameModel* Trampas[2];

	ResourceCollection CamionetaTextures;
	GameModel* Camioneta;

	ResourceCollection GarageTextures;
	GameModel* Garage;

	ResourceCollection HenoTextures;
	GameModel* Heno;

	ResourceCollection TroncoTextures;
	GameModel* Tronco;

	ColArray Colisiones;

#pragma endregion

	float movetext = 0;

	std::vector<ObjectPositions> trampasPosiciones;
	bool saved = false;
	float savedmsgtimer = 0.0f;
	std::vector<Trampa*> trampas;
	bool actualizarPosiciones = false;

	GUI* vida[3];
	int vidas = 3;
	int puntos = 0;
	bool invulnerable = false;
	float tiempo_inv = 0.0f;
	bool colisionando = false;

	GUI* gallinasHUD[4];

	GUI* itemSemillas[2];

	GUI* gameOver;

	GUI* victory;

	Text* texto;
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

	float posiciones[2] = { -40.0f, 20.0f};
	float rotationModel = 0.0f;

	float rotatecar;
	bool first = true;
	bool drive = false;
	bool rangoCamioneta = false;
	bool mount = false;

	float segundos = 301;

	void addTex(ResourceCollection& col, int number, const wchar_t* texture) {
		col.emplace_back(number, texture);
	}
	
	void addColisionBox(ColArray& arr, fvec3 origin, fvec3 scale) {
		arr.emplace_back(origin, scale);
	}

	void SavePosition() {
		ObjectPositions pos;
		pos.x = Jugador->GetPos().x;
		pos.y = Jugador->GetPos().y;
		pos.z = Jugador->GetPos().z;
		std::string space(" ");
		
		saved = true;

		saveinFile(CUSTOMFILENAME, pos);
		savedmsgtimer = 10.0f;
	}

	void GetPositions() {
		openandRead(CUSTOMFILENAME, trampasPosiciones);
		if (trampas.size() > 0)
		{
			trampas.clear();
			for (auto pos : trampasPosiciones) {
				trampas.push_back(new Trampa(Trampas[0], Trampas[1], camara, fvec3(pos.x, pos.y, pos.z), 2.0f));
			}
		}
		actualizarPosiciones = false;
	}

	float GetRadians(float a) {
		return (a * (D3DX_PI / 180));
	}

	void MontarVehiculo() {
		if(rangoCamioneta || !first) first = !first;
	}

    DXRR(HWND hWnd, int Ancho, int Alto)
	{
		GetPositions();
		#pragma region Inicializacion de elementos
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
		terreno = new TerrenoRR(1200, 1200, d3dDevice, d3dContext);
		skydome = new SkyDome(32, 32, 800.0f, &d3dDevice, &d3dContext, L"SKYD1.png", L"SKYD2.png", L"SKYD3.png");
		Aguita = new AguaRR(2000, 2000, d3dDevice, d3dContext);

		#pragma endregion
		#pragma region Modelos Inicializados

		CharacterTextures.reserve(2);
		addTex(CharacterTextures, 0, L"Assets/personaje/personajeColor.png");
		addTex(CharacterTextures, 1, L"Assets/personaje/personajeSpec.png");

		Character = new GameModel(d3dDevice, d3dContext, "Assets/personaje/personaje.obj", fvec3(60, 0, 30), CharacterTextures);

		ChickenTextures.reserve(2);
		addTex(ChickenTextures, 0, L"Assets/gallina/gallina.png");
		addTex(ChickenTextures, 1, L"Assets/noSpecMap.jpg");

		Chicken = new GameModel(d3dDevice, d3dContext, "Assets/gallina/gallina.obj", fvec3(0, 0, 0), ChickenTextures);

		FoodTextures.reserve(2);
		addTex(FoodTextures, 0, L"Assets/Semillas/SemillasColor.png");
		addTex(FoodTextures, 1, L"Assets/noSpecMap.jpg");

		ChickenFood = new GameModel(d3dDevice, d3dContext, "Assets/Semillas/Semillas.obj", fvec3(0, 0, 0), FoodTextures);

		FoodBagTextures.reserve(2);
		addTex(FoodBagTextures, 0, L"Assets/Bolsa/BolsaColor.png");
		addTex(FoodBagTextures, 1, L"Assets/noSpecMap.jpg");

		FoodBag = new GameModel(d3dDevice, d3dContext, "Assets/Bolsa/Bolsa.obj", fvec3(0, 0, 0), FoodBagTextures);

		SiloTextures.reserve(2);
		addTex(SiloTextures, 0, L"Assets/Silo/SiloColor.png");
		addTex(SiloTextures, 1, L"Assets/noSpecMap.jpg");

		Silo = new GameModel(d3dDevice, d3dContext, "Assets/Silo/Silo.obj", fvec3(40, 0, 20), SiloTextures);

		GraneroTextures.reserve(2);
		addTex(GraneroTextures, 0, L"Assets/Granero/GraneroColor.png");
		addTex(GraneroTextures, 1, L"Assets/noSpecMap.jpg");

		Granero = new GameModel(d3dDevice, d3dContext, "Assets/Granero/Granero.obj", fvec3(0, 0, 0), GraneroTextures);

		CamionetaTextures.reserve(2);
		addTex(CamionetaTextures, 0, L"Assets/camioneta/camionetaColor.jpg");
		addTex(CamionetaTextures, 1, L"Assets/camioneta/camionetaSpec.jpg");

		Camioneta = new GameModel(d3dDevice, d3dContext, "Assets/camioneta/camioneta.obj", fvec3(0, 0, 0), CamionetaTextures);

		GarageTextures.reserve(2);
		addTex(GarageTextures, 0, L"Assets/garage/garageColor.jpg");
		addTex(GarageTextures, 1, L"Assets/noSpecMap.jpg");

		Garage = new GameModel(d3dDevice, d3dContext, "Assets/garage/garage.obj", fvec3(0, 0, 0), GarageTextures);

		HenoTextures.reserve(2);
		addTex(HenoTextures, 0, L"Assets/heno/henoColor.png");
		addTex(HenoTextures, 1, L"Assets/noSpecMap.jpg");

		Heno = new GameModel(d3dDevice, d3dContext, "Assets/heno/heno.obj", fvec3(0, 0, 0), HenoTextures);
		
		TroncoTextures.reserve(2);
		addTex(TroncoTextures, 0, L"Assets/tronco/troncoColor.jpg");
		addTex(TroncoTextures, 1, L"Assets/tronco/TroncoSpec.jpg");

		Tronco = new GameModel(d3dDevice, d3dContext, "Assets/tronco/tronco.obj", fvec3(0, 0, 0), TroncoTextures);

		TrampaTextures.reserve(2);
		addTex(TrampaTextures, 0, L"Assets/Trampa/textrampa.png");
		addTex(TrampaTextures, 1, L"Assets/noSpecMap.jpg");

		Trampas[0] = new GameModel(d3dDevice, d3dContext, "Assets/Trampa/TrampaAbierta.obj", fvec3(0,0,0), TrampaTextures);
		Trampas[1] = new GameModel(d3dDevice, d3dContext, "Assets/Trampa/TrampaCerrada.obj", fvec3(0,0,0), TrampaTextures);

		#pragma endregion
		#pragma region Inizializacion de Elementos de Gameplay

		for (auto pos : trampasPosiciones) {
			trampas.push_back(new Trampa(Trampas[0], Trampas[1], camara, fvec3(pos.x, pos.y, pos.z), 2.0f));
		}

		Jugador = new Player(Character, camara, fvec3(2.0f, 10.0f, 2.0f));

		chickenOne = new Gallina(Chicken, fvec3(1.0f, 1.0f, 1.0f), 70.0f, 1);
		chickenTwo = new Gallina(Chicken, fvec3(1.0f, 1.0f, 1.0f), 70.0f, 2);
		chickenThree = new Gallina(Chicken, fvec3(1.0f, 1.0f, 1.0f), 70.0f, 3);

		chickenOne->SetPos(fvec3(10.0f, 0.0f, 80.0f));
		chickenTwo->SetPos(fvec3(40.0f, 0.0f, 80.0f));
		chickenThree->SetPos(fvec3(80.0f, 0.0f, 80.0f));

		item = new Item(ChickenFood, 10.0f);
		item->SetPos(fvec3(187.0f, 0, 300.0f));

		#pragma endregion
		
		camaraTipo = true;
		rotCam = 0.0f;

		#pragma region Inizializacion de Elementos de Interfaz

		XMFLOAT4 verde = XMFLOAT4(0.2, 0.7, 0.2, 0.5);
		WinTarget = new Primitive(d3dDevice, d3dContext, "Assets/Primitives/Cylinder.obj", D3DXVECTOR3(0, 0, 0), 5.0f, verde);

		vida[0] = new GUI(d3dDevice, d3dContext, 0.15, 0.26, L"Assets/UI/health_full.png");
		vida[1] = new GUI(d3dDevice, d3dContext, 0.15, 0.26, L"Assets/UI/health_2.png");
		vida[2] = new GUI(d3dDevice, d3dContext, 0.15, 0.26, L"Assets/UI/health_1.png");
		
		gallinasHUD[0] = new GUI(d3dDevice, d3dContext, 0.65, 0.46, L"Assets/UI/gallinas_0.png");
		gallinasHUD[1] = new GUI(d3dDevice, d3dContext, 0.65, 0.46, L"Assets/UI/gallinas_1.png");
		gallinasHUD[2] = new GUI(d3dDevice, d3dContext, 0.65, 0.46, L"Assets/UI/gallinas_2.png");
		gallinasHUD[3] = new GUI(d3dDevice, d3dContext, 0.65, 0.46, L"Assets/UI/gallinas_3.png");

		itemSemillas[0] = new GUI(d3dDevice, d3dContext, 0.45, 0.25, L"Assets/UI/item_inactivo.png");
		itemSemillas[1] = new GUI(d3dDevice, d3dContext, 0.45, 0.25, L"Assets/UI/item_activo.png");

		gameOver = new GUI(d3dDevice, d3dContext, 0.55, 0.55, L"Assets/UI/game_over.png");

		victory = new GUI(d3dDevice, d3dContext, 0.65, 0.75, L"Assets/UI/you_win.png");

		texto = new Text(d3dDevice, d3dContext, 3.6, 1.2, L"Assets/UI/font.jpg", XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));

		#pragma endregion

		Granero->setPos(fvec3(320.0f, 0.0f, 380.0f));
		Granero->setYRot(GetRadians(180.0f));
		Granero->setAltura(terreno->Superficie(Granero->getX(), Granero->getZ()));

		Silo->setPos(fvec3(175.0f, 0.0f, 320.0f));
		Silo->setYRot(GetRadians(240.0f));
		Silo->setAltura(terreno->Superficie(Silo->getX(), Silo->getZ()));

		Garage->setPos(fvec3(405.0f, 0.0f, 200.0f));
		Garage->setYRot(GetRadians(270.0f));
		Garage->setAltura(terreno->Superficie(Garage->getX(), Garage->getZ()));

		Camioneta->setPos(fvec3(360.0f, 0.0f, 200.0f));
		Camioneta->setYRot(GetRadians(90.0f));
		Camioneta->setAltura(terreno->Superficie(Camioneta->getX(), Camioneta->getZ()));

		Heno->setPos(fvec3(285.0f, 0.0f, 360.0f));
		Heno->setYRot(GetRadians(180.0f));
		Heno->setAltura(terreno->Superficie(Heno->getX(), Heno->getZ()));

		Tronco->setPos(fvec3(190.0f, 0.0f, -35.0f));
		Tronco->setYRot(GetRadians(295.0f));
		Tronco->setAltura(terreno->Superficie(Tronco->getX(), Tronco->getZ()));

		WinTarget->setPos(fvec3(320.0f, 0.0f, 335.0f));
		WinTarget->setAltura(terreno->Superficie(WinTarget->getX(), WinTarget->getZ()));
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
		initIMGUI(hWnd);

		bool res = m_XACT3.Initialize();
		if (!res) return false;
		res = m_XACT3.LoadWaveBank(L"Assets\\Sonido\\gamebank.xwb");
		if (!res) return false;
		res = m_XACT3.LoadSoundBank(L"Assets\\Sonido\\soundbank.xsb");
		if (!res) return false;

		cueIndex = m_XACT3.m_pSoundBank->GetCueIndex("birds_background");
		m_XACT3.m_pSoundBank->Play(cueIndex, 0, 0, 0);
		return true;			
		
	}
	bool walking = false;
	bool chickenIsFollowing = false;

	void startWalkingSfx() {
		if (!walking) {
			cueIndex = m_XACT3.m_pSoundBank->GetCueIndex("walk_sound");
			m_XACT3.m_pSoundBank->Play(cueIndex, 0, 0, 0);
			walking = true;
		}
	}

	void stopWalkingSfx() {
		if (walking) {
			cueIndex = m_XACT3.m_pSoundBank->GetCueIndex("walk_sound");
			m_XACT3.m_pSoundBank->Stop(cueIndex, 0);
			walking = false;
		}
	}

	void startChickenSfx() {
		if (!chickenIsFollowing) {
			cueIndex = m_XACT3.m_pSoundBank->GetCueIndex("chicken_sound");
			m_XACT3.m_pSoundBank->Play(cueIndex, 0, 0, 0);
			chickenIsFollowing = true;
		}
	}

	void stopChickenSfx() {
		if (chickenIsFollowing) {
			cueIndex = m_XACT3.m_pSoundBank->GetCueIndex("chicken_sound");
			m_XACT3.m_pSoundBank->Stop(cueIndex, 0);
			chickenIsFollowing = false;
		}
	}

	void alternarItem() {
		Jugador->toggleItem();
	}

	void LiberaD3D(void)
	{
		delete Character;
		delete Chicken;
		delete ChickenFood;
		delete FoodBag;
		delete Silo;
		delete Granero;
		delete Trampas[0];
		delete Trampas[1];
		delete Camioneta;
		delete Garage;
		delete Heno;
		delete Tronco;

		CharacterTextures.clear();
		ChickenTextures.clear();
		FoodTextures.clear();
		FoodBagTextures.clear();
		SiloTextures.clear();
		TrampaTextures.clear();
		GraneroTextures.clear();
		CamionetaTextures.clear();
		GarageTextures.clear();
		HenoTextures.clear();
		TroncoTextures.clear();

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
	}
	
	void Update(void) {
		if ((vel > 0 || vel < 0) && !drive) startWalkingSfx();
		else stopWalkingSfx();

		if (actualizarPosiciones) GetPositions();

		if (d3dContext == 0)
			return;

		rotCam += izqder;

		float clearColor[4] = { 0, 0, 0, 1.0f };
		d3dContext->ClearRenderTargetView(backBufferTarget, clearColor);
		d3dContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		camara->posCam.y = terreno->Superficie(camara->posCam.x, camara->posCam.z) + 17;
		camara->posCam3P.y = terreno->Superficie(camara->posCam3P.x, camara->posCam3P.z) + 17;
		//camara->UpdateCam(vel, arriaba, izqder);
		skydome->Update(camara->vista, camara->proyeccion);

		//float camPosXZ[2] = { camara->posCam.x, camara->posCam.z };
		float x = Camioneta->getX();
		float z = Camioneta->getZ();
		float y = terreno->Superficie(x, z);
		
		if (first) camara->UpdateCam(vel, arriaba, izqder);
		else camara->UpdateCam2(vel, arriaba, izqder, D3DXVECTOR3(x, y, z));

		float camPosXZ[2] = { camara->posCam.x, camara->posCam.z };
		x = camara->posCam.x;
		z = camara->posCam.z;
		y = terreno->Superficie(x, z);

		#pragma region Gameplay Stuff

		item->SetAltura(terreno->Superficie(item->getX(), item->getZ()));
		item->Update();
		
		fvec3 pos = fvec3(x, y, z);
		Jugador->Update(pos, Colisiones);
		bool obtenido = false;
		Jugador->obtenerItem(item, obtenido);
		if (obtenido)
		{
			cueIndex = m_XACT3.m_pSoundBank->GetCueIndex("pickup_sound");
			m_XACT3.m_pSoundBank->Play(cueIndex, 0, 0, 0);
		}
		if (Jugador->CajaDeColision->CheckSphereColission(Camioneta->getPos(), 25.0f))
			rangoCamioneta = true;
		else 
			rangoCamioneta = false;

		bool siguiendo1 = false;
		chickenOne->Update(Jugador, chickenOne->GetPos(), siguiendo1);
		chickenOne->SetAltura(terreno->Superficie(chickenOne->GetPos().x, chickenOne->GetPos().z));

		bool siguiendo2 = false;
		chickenTwo->Update(Jugador, chickenTwo->GetPos(), siguiendo2);
		chickenTwo->SetAltura(terreno->Superficie(chickenTwo->GetPos().x, chickenTwo->GetPos().z));

		bool siguiendo3 = false;
		chickenThree->Update(Jugador, chickenThree->GetPos(), siguiendo3);
		chickenThree->SetAltura(terreno->Superficie(chickenThree->GetPos().x, chickenThree->GetPos().z));

		if (siguiendo1 || siguiendo2 || siguiendo3) startChickenSfx();
		else stopChickenSfx();

		Gallina* gallinas[3];
		gallinas[0] = chickenOne;
		gallinas[1] = chickenTwo;
		gallinas[2] = chickenThree;

		for (auto trampa : trampas) 
		{
			trampa->Update(gallinas, Jugador);
		}

		item->Update();

		WinTarget->Update(gallinas, Jugador, item);

		#pragma endregion

		if (false) {
			//WinTarget->setPos(fvec3(posiciones[0], terreno->Superficie(posiciones[0], posiciones[1]), posiciones[1]));
			//Camioneta->setYRot(rotationModel);
		}
		static float angbuff = -camara->ang2 + 90;

		if (drive == true) {
			if (angbuff < -camara->ang2 + 91) {
				Camioneta->setYRot(angbuff);
				angbuff += 2;
			}

			if (angbuff > -camara->ang2 + 89) {
				Camioneta->setYRot(angbuff);
				angbuff -= 2;
			}

			if (angbuff == -camara->ang2 + 90) Camioneta->setYRot(angbuff);
			Camioneta->mPosicion.x = camara->Camaracontra().x;
			Camioneta->mPosicion.z = camara->Camaracontra().z;
			Camioneta->mPosicion.y = terreno->Superficie(camara->Camaracontra().x, camara->Camaracontra().z);
		}

		/*
		Tronco->mPosicion.x = xc;
		Tronco->mPosicion.z = 0;
		Tronco->mPosicion.y = yc + terreno->Superficie(xc, 0);
		*/

	}

	int packnumber = 0;
	void Render(void)
	{
		if (d3dContext == 0)
			return;

		static float lerpDay = 5.0f;
		static int DayFase = 0;
						
	    lerpDay += 0.003;
		if (lerpDay >= 1.0f) {
			lerpDay = 0.0f;
			DayFase++;
		}

		if (DayFase > 5) DayFase = 0;

		TurnOffDepth();
		skydome->Render(camara->posCam, DayFase, lerpDay);
		TurnOnDepth();

		XMFLOAT3 m_camPos;
		m_camPos.x = camara->posCam.x;
		m_camPos.y = camara->posCam.y;
		m_camPos.z = camara->posCam.z;

		static float ang = 90;
		ang += 0.18;

		float xc = (500 * (cos(ang * 3.1416 / 180)));
		float yc = (500 * (sin(ang * 3.1416 / 180)));

		if (ang >= 360) ang = 0;

		XMFLOAT3 m_lightPos;
		m_lightPos.x = xc;
		m_lightPos.y = yc + terreno->Superficie(xc,0.0f);
		m_lightPos.z = 0.0f;

		terreno->Draw(camara->vista, camara->proyeccion, m_camPos, m_lightPos);

		movetext += 0.0025;

		static float wave = 0;
		wave += 0.01f;
		float valorw = 0.5f * (5 + sin(wave));
		//TurnOnAlphaBlending();
		//billboard->Draw(camara->vista, camara->proyeccion, camara->posCam,
			//-11, -78, 4, 5, uv1, uv2, uv3, uv4, frameBillboard);
		//TurnOffAlphaBlending();

		#pragma region My Drawing Stuff
		//Models
		Granero->Draw(camara, 1.0f, 1.0f);
		Heno->Draw(camara, 1.0f, 1.0f);
		Tronco->Draw(camara, 1.0f, 1.0f);
		Garage->Draw(camara, 1.0f, 1.0f);
		Camioneta->Draw2(camara, 1.0f, 1.0f);
		Silo->Draw(camara, 1.0f, 1.0f);

		//Gameplay Elements
		Jugador->Draw(camara, 1.0f, 1.0f);
		chickenOne->Draw(camara, 1.0f, 1.0f);
		chickenTwo->Draw(camara,  1.0f, 1.0f);
		chickenThree->Draw(camara, 1.0f, 1.0f);

		TurnOnAlphaBlending();
		item->Draw(camara, 1.0f, 1.0f);
		TurnOffAlphaBlending();

		Granero->Draw(camara, 1.0f, 1.0f);
		Heno->Draw(camara, 1.0f, 1.0f);
		Tronco->Draw(camara, 1.0f, 1.0f);
		Garage->Draw(camara, 1.0f, 1.0f);
		Camioneta->Draw2(camara, 1.0f, 1.0f);
		Silo->Draw(camara, 1.0f, 1.0f);

		TurnOnAlphaBlending();
			WinTarget->Draw(camara, 1.0f);
		TurnOffAlphaBlending();

		for (auto trampa : trampas) {
			trampa->Draw(camara, 1.0f, 1.0f);
		}

		TurnOnAlphaBlending();
	    Aguita->Draw(camara->vista, camara->proyeccion, movetext, m_camPos, m_lightPos);
		TurnOffAlphaBlending();
		#pragma endregion

		#pragma region UI Stuff

		if (Jugador->itemOnHand) itemSemillas[1]->Draw(0.85, 0.75);
		else itemSemillas[0]->Draw(0.85, 0.75);

		if (vidas == 3) vida[0]->Draw(0.75, -0.75);
		else if (vidas == 2) vida[1]->Draw(0.75, -0.75);
		else if (vidas == 1) vida[2]->Draw(0.75, -0.75);
		else if (vidas == 0) gameOver->Draw(0.0, 0.0);
		
		if(chickenOne->GetFallInTrap()) gameOver->Draw(0.0, 0.0);
		if(chickenTwo->GetFallInTrap()) gameOver->Draw(0.0, 0.0);
		if(chickenThree->GetFallInTrap()) gameOver->Draw(0.0, 0.0);

		std::string num = "Number: " + std::to_string(packnumber);
		texto->DrawsText(-0.10, 0, num.c_str(), 0.005);
		std::string pts = "Puntos: " + std::to_string(Jugador->puntos);
		texto->DrawsText(-0.80, -0.55, pts.c_str(), 0.005);
		std::string montar = "Presiona \"E\" para Manejar";
		if (rangoCamioneta) texto->DrawsText(-0.15, -0.6, montar.c_str(), 0.005);
		if (Jugador->itemOnInventory) {
			std::string textocontrol = "Presiona \"Q\" para Alternar Semillas";
			texto->DrawsText(-0.20, -0.6, textocontrol.c_str(), 0.005);
		}
		segundos -= 0.02;

		if (Jugador->puntos == 0) gallinasHUD[0]->Draw(-0.75, -0.75);
		if (Jugador->puntos == 1) gallinasHUD[1]->Draw(-0.75, -0.75);
		if (Jugador->puntos == 2) gallinasHUD[2]->Draw(-0.75, -0.75);
		if (Jugador->puntos == 3) {
			gallinasHUD[3]->Draw(-0.75, -0.75);
			victory->Draw(0.0, 0.0);
		}

		#pragma endregion

		if (rotationModel > 360.0f) {
			rotationModel = 0.0f;
		}
		else if (rotationModel < 0.0f) {
			rotationModel = 360.0f;
		}

		saved = savedmsgtimer >= 0.0f;
		if (saved) {
			savedmsgtimer -= 0.1f;
		}
		#pragma region ImGui Debug Stuff

		if (false) {

			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
			ImGui::Begin("Ventana de pruebas!");
			if (saved) ImGui::Text("Saved");
			std::string col = "puntos: " + std::to_string(Jugador->puntos);
			ImGui::Text(col.c_str());
			ImGui::Text("Configuracion del posiciones");
			ImGui::DragFloat2("Translation X / Z", posiciones, 5.0f, -500.0f, 500.0f);
			ImGui::DragFloat("Rotacion Objeto", &rotationModel, 1.0f, 0, 36.0f);
			std::string playerPositionMsg = "Posicion del personaje X, Y, Z\n" + std::to_string(Jugador->GetPos().x) + ", " + std::to_string(Jugador->GetPos().y) + ", " + std::to_string(Jugador->GetPos().z);
			ImGui::Text(playerPositionMsg.c_str());
			ImGui::End();

			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		}
		#pragma endregion

		swapChain->Present( 1, 0 );
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

private:

	void initIMGUI(HWND hwnd) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(d3dDevice, d3dContext);
		ImGui::StyleColorsDark();
	}
};
#endif