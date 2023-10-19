#pragma once
#include <Windows.h>
#include <WindowsX.h>
#include <functional>
#include <unordered_map>

using KeyFunc = std::function<void()>;

enum class KeyMap : UINT8
{
	KeyBoardStart = 0,
	KeyBoardEnd = 127,

	MouseLeftClick = 128,
	MouseRightClick = MouseLeftClick + 1,
	MouseMiddleClick = MouseRightClick + 1,
	MouseMiddleUp = MouseMiddleClick + 1,
	MouseMiddleDown = MouseMiddleUp + 1,
};

class InputManager
{
public:
	~InputManager();

	static InputManager& Get();

	void BindKey(UINT8 InKey, KeyFunc InKeyDownFunc, KeyFunc InKeyUpFunc);
	void BindKey(KeyMap InKey, KeyFunc InKeyDownFunc, KeyFunc InKeyUpFunc);

	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	InputManager();

	virtual void OnKeyDown(UINT8 InKey);
	virtual void OnKeyUp(UINT8 InKey);

	bool IsKeyBoard(UINT8 InKey);

private:
	std::unordered_map<UINT8, KeyFunc> KeyDownMap;
	std::unordered_map<UINT8, KeyFunc> KeyUpMap;
};