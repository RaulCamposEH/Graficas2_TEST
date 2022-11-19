#include <windows.h>
#include <windowsx.h>

#include "DXRR.h"
#include "GamePadRR.h"

#include <dinput.h>
#include <xinput.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

#define SCREEN_X 1920
#define SCREEN_Y 1080

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

DXRR *dxrr;
GamePadRR *gamePad;
tagPOINT initialPoint;
tagPOINT actualPoint;
LPDIRECTINPUT8 m_pDirectInput = NULL;
LPDIRECTINPUTDEVICE8 m_pKeyboardDevice = NULL;
LPDIRECTINPUTDEVICE8 m_pMouseDevice = NULL;
bool stillpressed = false;

void createMouseDevice(HWND hWnd) {
    m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouseDevice, 0);
    m_pMouseDevice->SetDataFormat(&c_dfDIMouse);
    m_pMouseDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    m_pMouseDevice->Acquire();
}

void createKeyboardDevice(HWND hWnd) {
    m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboardDevice, 0);
    m_pKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
    m_pKeyboardDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    m_pKeyboardDevice->Acquire();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    HWND hWnd;
    WNDCLASSEX wc;
    DEVMODE dmScreenSettings;
    int posX, posY;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"DXRR_E1";
    wc.cbSize = sizeof(WNDCLASSEX);

    RegisterClassEx(&wc);

    // If full screen set the screen to maximum size of the users desktop and 32bit.
    memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
    dmScreenSettings.dmSize = sizeof(dmScreenSettings);
    dmScreenSettings.dmPelsWidth = (unsigned long)SCREEN_X;
    dmScreenSettings.dmPelsHeight = (unsigned long)SCREEN_Y;
    dmScreenSettings.dmBitsPerPel = 32;
    dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

    // Change the display settings to full screen.
    ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

    // Set the position of the window to the top left corner.
    posX = posY = 0;

    RECT wr = {0, 0, SCREEN_X, SCREEN_Y};
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
    hWnd = CreateWindowEx(
        WS_EX_APPWINDOW, 
        L"DXRR_E1", 
        L"PLANTILLA PROYECTO", 
        WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
        posX, posY, SCREEN_X, SCREEN_Y,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
	dxrr = new DXRR(hWnd, 800, 600);
	dxrr->vel=0;
    gamePad = new GamePadRR(1);

    ClientToScreen(hWnd, &initialPoint);
    actualPoint.x = initialPoint.x + SCREEN_X / 2;
    actualPoint.y = initialPoint.y + SCREEN_Y / 2;

	SetTimer(hWnd, 100, 33, NULL);
    MSG msg;
    ::DirectInput8Create(
        hInstance, DIRECTINPUT_VERSION,
        IID_IDirectInput8, (void**)&m_pDirectInput, 0);
    createMouseDevice(hWnd);
    createKeyboardDevice(hWnd);
    while(TRUE)
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if(msg.message == WM_QUIT)
                break;
        }
        dxrr->Update();
        dxrr->Render();
    }

    if (m_pMouseDevice) {
        m_pMouseDevice->Unacquire();
        m_pMouseDevice->Release();
    }
    m_pMouseDevice = NULL;

    if (m_pKeyboardDevice) {
        m_pKeyboardDevice->Unacquire();
        m_pKeyboardDevice->Release();
    }
    m_pKeyboardDevice = NULL;

    if (m_pDirectInput)
        m_pDirectInput->Release();
    m_pDirectInput = NULL;

    return msg.wParam;
}

char keyboardData[256];
bool init = false;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        //return true;

    float xPos = 0;
    float yPos = 0;

    if (init) {
        m_pKeyboardDevice->GetDeviceState(sizeof(keyboardData), (void*)&keyboardData);
    }

    switch(message)
    {

        case WM_DESTROY:
        {
			KillTimer(hWnd, 100);
            PostQuitMessage(0);
            return 0;
            break;
        } 
		case WM_TIMER:
		{

            break;
		} 
        case WM_KEYDOWN:
        {
            if (keyboardData[DIK_E]) {
                dxrr->MontarVehiculo();
            }
            break;
        }
        case WM_KEYUP: 
        {

            switch (wParam)
            {
                case VK_F1:
                {
                    dxrr->SavePosition();
                    break;
                }
                case VK_F2:
                {
                    dxrr->actualizarPosiciones = true;
                    break;
                }
            }
            break;
        } 
        case WM_MOUSEMOVE: 
        {
            SetCursorPos(actualPoint.x, actualPoint.y);
            dxrr->frameBillboard++;
            if (dxrr->frameBillboard == 32)
                dxrr->frameBillboard = 0;

            dxrr->izqder = 0;
            dxrr->arriaba = 0;
            dxrr->vel = 0;

            if(!init) init = true;
   

            if (keyboardData[DIK_SPACE] & 0x80) {
                dxrr->vel = 15.f;
                dxrr->drive = true;
            }
            else {
                dxrr->drive = false;
            }

           
            if (keyboardData[DIK_S] & 0x80) {
                dxrr->vel = -5.f;
                dxrr->vel = -10.f;
            }
            if (keyboardData[DIK_W] & 0x80) {
                dxrr->vel = 5.f;
                dxrr->vel = 10.f;
            }
            if (keyboardData[DIK_A] & 0x80) {
                dxrr->rotatecar = -5.f;
            }
            if (keyboardData[DIK_D] & 0x80) {
                dxrr->rotatecar = 5.f;
            }
            if (keyboardData[DIK_Z] & 0x80) {
                dxrr->first = true;
            }
            if (keyboardData[DIK_X] & 0x80) {
                dxrr->first = false;
            }
            if (keyboardData[DIK_B] & 0x80) {
                dxrr->breakpoint = true;
            }
            if (keyboardData[DIK_ESCAPE] & 0x80) {
                KillTimer(hWnd, 100);
                PostQuitMessage(0);
                return 0;
            }

            if (keyboardData[DIK_UP] & 0x80) {
                dxrr->posiciones[1] += 2.0f;
                dxrr->rotationModel = 0.0f;
            }
            if (keyboardData[DIK_DOWN] & 0x80) {
                dxrr->posiciones[1] -= 2.0f;
                dxrr->rotationModel = 180.0f;
            }
            if (keyboardData[DIK_LEFT] & 0x80) {
                dxrr->posiciones[0] -= 2.0f;
                dxrr->rotationModel = 270.0f;
            }
            if (keyboardData[DIK_RIGHT] & 0x80) {
                dxrr->posiciones[0] += 2.0f;
                dxrr->rotationModel = 90.0f;
            }

            if (keyboardData[DIK_ADD] & 0x80) {
                dxrr->rotationModel += 5.0f;
            }
            if (keyboardData[DIK_SUBTRACT] & 0x80) {
                dxrr->rotationModel -= 5.0f;
            }



            DIMOUSESTATE mouseData;
            m_pMouseDevice->GetDeviceState(sizeof(mouseData), (void*)&mouseData);

            // Mouse move
            dxrr->izqder = (mouseData.lX / 1000.0f);
            dxrr->arriaba = -(mouseData.lY / 1000.0f);

            if (gamePad->IsConnected())
            {
                float grados = (float)gamePad->GetState().Gamepad.sThumbRX / 32767.0;
                if (grados > 0.19 || grados < -0.19) dxrr->izqder = grados / 15;
                grados = (float)gamePad->GetState().Gamepad.sThumbRY / 32767.0;
                if (grados > 0.19 || grados < -0.19) dxrr->arriaba = grados / 15;
                float velocidad = (float)gamePad->GetState().Gamepad.sThumbLY / 32767.0;
                if (velocidad > 0.19 || velocidad < -0.19) {
                    if (gamePad->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) velocidad *= 14.5;
                    else if (gamePad->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) velocidad /= 3;
                    else velocidad *= 5.5;
                    if (velocidad > 0.19) dxrr->vel = velocidad;
                    else if (velocidad < -0.19) dxrr->vel = velocidad;
                }
                if (gamePad->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A && !stillpressed) {
                    if (!stillpressed) dxrr->SavePosition();
                    stillpressed = true;
                }
                else stillpressed = false;
            }

            break;
        }

    }

    return DefWindowProc (hWnd, message, wParam, lParam);
}



