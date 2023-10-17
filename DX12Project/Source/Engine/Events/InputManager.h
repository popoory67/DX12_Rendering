#pragma once
#include <Windows.h>
#include <WindowsX.h>
#include <functional>
#include <unordered_map>

using KeyFunc = std::function<void()>;

class InputManager
{
public:
	~InputManager();

	static InputManager& Get();

	void BindKey(int InKey, KeyFunc InFunc);

	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	InputManager();

	virtual void OnKeyDown(UINT8 InKey);
	virtual void OnKeyUp(UINT8 InKey);

private:
	std::unordered_map<int, KeyFunc> KeyMap;
};