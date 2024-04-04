#include "WinDisplay.h"


#include <iostream>

#include "Engine.h"
#include "../Core/Input/InputDevice.h"

WinDisplay::WinDisplay()
{
}

WinDisplay::~WinDisplay()
{
}

LRESULT WinDisplay::WindowProcedure(HWND handlerWindow, uint32_t message, UINT_PTR uintParam, LONG_PTR intParam)
{
    return Engine::GetInstance().GetInputDevice()->HandleInput(handlerWindow, message, uintParam, intParam);
}


void WinDisplay::WindowInit()
{
    applicationName = L"CGProj";
    hInstance = GetModuleHandle(nullptr);

    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WindowProcedure;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO); // IDI_WINLOGO
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW); // IDC_NO IDC_ARROW
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = applicationName;
    wc.cbSize = sizeof(WNDCLASSEX);

    // Register the window class.
    RegisterClassEx(&wc);

    screenWidth = screenWidth > 0 ? screenWidth : 1000;
    screenHeight = screenHeight > 0 ? screenHeight : 1000;

    RECT windowRect = {0, 0, static_cast<LONG>(screenWidth), static_cast<LONG>(screenHeight)};
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    auto dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME;

    posX = posX > 0 ? posX : (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
    posY = posY > 0 ? posY : (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

    hWnd = CreateWindowEx(WS_EX_APPWINDOW, applicationName, applicationName, dwStyle, posX, posY, windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top, nullptr, nullptr, hInstance, nullptr);

    ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd);
    SetFocus(hWnd);

    ShowCursor(false);
}
