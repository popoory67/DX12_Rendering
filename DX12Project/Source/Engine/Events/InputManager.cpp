#include "InputManager.h"
#include <assert.h>

InputManager* InputManager::Instance = nullptr;

InputManager::InputManager()
{
	Timer = new GameTimer();
	assert(Timer);
}

InputManager::~InputManager()
{
}

InputManager& InputManager::Get()
{
	if (!Instance)
	{
		Instance = new InputManager();
	}

	return *Instance;
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

class GameTimer* InputManager::GetTimer() const
{
	return Timer;
}
