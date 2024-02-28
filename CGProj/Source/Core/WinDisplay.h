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

    static LRESULT CALLBACK WindowProcedure(HWND handlerWindow, uint32_t message, UINT_PTR uintParam, LONG_PTR intParam);
};
