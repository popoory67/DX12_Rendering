#include "InputManager.h"

InputManager::InputManager()
{
}

InputManager::~InputManager()
{
}

InputManager& InputManager::Get()
{
    static InputManager Instance;
    return Instance;
}

void InputManager::BindKey(UINT8 InKey, KeyFunc InKeyDownFunc, KeyFunc InKeyUpFunc)
{
    KeyDownMap.emplace(InKey, InKeyDownFunc);
    KeyUpMap.emplace(InKey, InKeyUpFunc);
}

void InputManager::BindKey(KeyMap InKey, KeyFunc InKeyDownFunc, KeyFunc InKeyUpFunc)
{
    BindKey(static_cast<UINT8>(InKey), InKeyDownFunc, InKeyUpFunc);
}

void InputManager::OnKeyDown(UINT8 InKey)
{
    if (!KeyDownMap[InKey])
    {
        return;
    }

    KeyDownMap[InKey]();
}

void InputManager::OnKeyUp(UINT8 InKey)
{
    if (!KeyUpMap[InKey])
    {
        return;
    }

    KeyUpMap[InKey]();
}

bool InputManager::IsKeyBoard(UINT8 InKey)
{
    return static_cast<KeyMap>(InKey) > KeyMap::KeyBoardEnd;
}

LRESULT InputManager::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
    case WM_KEYDOWN:
        OnKeyDown(static_cast<UINT8>(wParam));
        return 0;

    case WM_KEYUP:
		OnKeyUp(static_cast<UINT8>(wParam));
        return 0;

    case WM_MOUSEWHEEL:
    {  
        int delta = GET_WHEEL_DELTA_WPARAM(wParam);
        UINT8 key = delta > 0 ? static_cast<UINT8>(KeyMap::MouseMiddleUp) : static_cast<UINT8>(KeyMap::MouseMiddleDown);

        OnKeyDown(key);
    }
    return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}