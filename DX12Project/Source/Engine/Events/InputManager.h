#pragma once

#include <Windows.h>
#include <WindowsX.h>
#include "GameTimer.h"

class InputManager
{
public:
	~InputManager();

	static InputManager& Get();

	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	class GameTimer* GetTimer() const;

private:

	InputManager();

	static InputManager* Instance;

	class GameTimer* Timer;
};