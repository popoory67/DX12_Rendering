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

void InputManager::BindKey(int InKey, KeyFunc InFunc)
{
    KeyMap.emplace(InKey, InFunc);
}

void InputManager::OnKeyDown(UINT8 InKey)
{
    if (!KeyMap[InKey])
    {
        return;
    }

    KeyMap[InKey]();

    //switch (InKey)
    //{
    //case 'W':
    //    break;
    //case 'A':
    //    break;
    //case 'S':
    //    break;
    //case 'D':
    //    break;
    //case VK_LEFT:
    //    break;
    //case VK_RIGHT:
    //    break;
    //case VK_UP:
    //    break;
    //case VK_DOWN:
    //    break;
    //case VK_ESCAPE:
    //    break;
    //}
}

void InputManager::OnKeyUp(UINT8 InKey)
{

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

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}