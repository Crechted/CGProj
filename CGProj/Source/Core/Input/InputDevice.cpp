#include "InputDevice.h"
#include <iostream>
#include "../../Core/Game.h"
#include "../../Core/WinDisplay.h"
#include "Inc/SimpleMath.h"

using  namespace DirectX::SimpleMath;

InputDevice::InputDevice(Game* inGame)
    : game(inGame)
{
    keys = new std::unordered_set<Keys>();

    RAWINPUTDEVICE Rid[2];

    Rid[0].usUsagePage = 0x01;
    Rid[0].usUsage = 0x02;
    Rid[0].dwFlags = 0; // adds HID mouse and also ignores legacy mouse messages
    Rid[0].hwndTarget = game->display->hWnd;

    Rid[1].usUsagePage = 0x01;
    Rid[1].usUsage = 0x06;
    Rid[1].dwFlags = 0; // adds HID keyboard and also ignores legacy keyboard messages
    Rid[1].hwndTarget = game->display->hWnd;

    if (!RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])))
    {
        auto errorCode = GetLastError();
        std::cout << "ERROR: " << errorCode << std::endl;
    }
}

RAWINPUT* InputDevice::ReadRawInput(uint32_t* pSize, LONG_PTR pIntParam)
{
    GetRawInputData(reinterpret_cast<HRAWINPUT>(pIntParam), RID_INPUT,
        inputBuffer_, pSize, sizeof(RAWINPUTHEADER));
    return reinterpret_cast<RAWINPUT*>(inputBuffer_);
}

LONG_PTR CALLBACK InputDevice::HandleInput(HWND handlerWindow, uint32_t message, UINT_PTR uintParam, LONG_PTR intParam)
{
    if (message != WM_INPUT) return DefWindowProc(handlerWindow, message, uintParam, intParam);

    uint32_t size;
    auto* pRawInput = ReadRawInput(&size, intParam);
    if (size == 0) return DefWindowProc(handlerWindow, message, uintParam, intParam);

    if (pRawInput->header.dwType == RIM_TYPEKEYBOARD) HanleKey(pRawInput);
    else if (pRawInput->header.dwType == RIM_TYPEMOUSE) HandleMouse(pRawInput);

    return DefWindowProc(handlerWindow, message, uintParam, intParam);
}

InputDevice::~InputDevice()
{
    delete keys;
}

void InputDevice::HanleKey(RAWINPUT* rawInput)
{
    //bool Break = rawInput.Flags & 0x01;

    auto key = static_cast<Keys>(rawInput->data.keyboard.VKey);

    if (rawInput->data.keyboard.MakeCode == 42) key = Keys::LeftShift;
    if (rawInput->data.keyboard.MakeCode == 54) key = Keys::RightShift;

    if (rawInput->data.keyboard.Flags & RI_KEY_BREAK)
    {
        if (keys->count(key)) RemovePressedKey(key);
    }
    else
    {
        if (!keys->count(key)) AddPressedKey(key);
    }
}

void InputDevice::HandleMouse(RAWINPUT* rawInput)
{
    const auto flags = rawInput->data.mouse.usButtonFlags;
    if (flags & static_cast<int>(MouseButtonFlags::LeftButtonDown))
        AddPressedKey(Keys::LeftButton);
    if (flags & static_cast<int>(MouseButtonFlags::LeftButtonUp))
        RemovePressedKey(Keys::LeftButton);
    if (flags & static_cast<int>(MouseButtonFlags::RightButtonDown))
        AddPressedKey(Keys::RightButton);
    if (flags & static_cast<int>(MouseButtonFlags::RightButtonUp))
        RemovePressedKey(Keys::RightButton);
    if (flags & static_cast<int>(MouseButtonFlags::MiddleButtonDown))
        AddPressedKey(Keys::MiddleButton);
    if (flags & static_cast<int>(MouseButtonFlags::MiddleButtonUp))
        RemovePressedKey(Keys::MiddleButton);

    POINT p;
    GetCursorPos(&p);
    ScreenToClient(game->display->hWnd, &p);

    MousePosition = Vector2(p.x, p.y);
    MouseOffset = Vector2(rawInput->data.mouse.lLastX, rawInput->data.mouse.lLastY);
    MouseWheelDelta = rawInput->data.mouse.usButtonData;

    const MouseMoveEventArgs moveArgs = {MousePosition, MouseOffset, MouseWheelDelta};

    /*printf(" Mouse: posX=%04.4f posY:%04.4f offsetX:%04.4f offsetY:%04.4f, wheelDelta=%04d \n",
        MousePosition.x,
        MousePosition.y,
        MouseOffset.x,
        MouseOffset.y,
        MouseWheelDelta);*/

    MouseMoveDelegate.Broadcast(moveArgs);
}

void InputDevice::AddPressedKey(Keys key)
{
    //if (!game->isActive) {
    //	return;
    //}
    keys->insert(key);    
    KeyDownDelegate.Broadcast(key);
}

void InputDevice::RemovePressedKey(Keys key)
{
    keys->erase(key);    
    KeyUpDelegate.Broadcast(key);
}

bool InputDevice::IsKeyDown(Keys key)
{
    return keys->count(key);
}
