#pragma once

#include <Windows.h>
#include <WindowsX.h>
#include <windows.h>
#include "Util/GameTimer.h"

class InputManager
{
public:
	~InputManager();

	static InputManager& GetInstance();

	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	GameTimer GetTimer() const { return Timer; }

private:

	InputManager();

	static InputManager* Instance;

	GameTimer Timer;
};

#define GAME_TIMER		InputManager::GetInstance().GetTimer()