#pragma once

#include <windows.h>
#include <WinUser.h>
#include <directxmath.h>

class WinDisplay
{
public:
    WinDisplay();
    virtual ~WinDisplay();    
    void WindowInit();

    LPCWSTR applicationName;
    HINSTANCE hInstance;    
    WNDCLASSEX wc;
    HWND hWnd;

    int32_t screenWidth;
    int32_t screenHeight;
};
