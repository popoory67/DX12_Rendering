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

LRESULT InputManager::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}