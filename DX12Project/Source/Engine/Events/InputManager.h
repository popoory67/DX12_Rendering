#pragma once
#include <Windows.h>
#include <WindowsX.h>

class InputManager
{
public:
	~InputManager();

	static InputManager& Get();

	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:

	InputManager();
};