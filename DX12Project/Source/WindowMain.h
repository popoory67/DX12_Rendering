#pragma once

#include <Windows.h>
#include <WindowsX.h>
#include <windows.h>

class WindowMain
{
public:

	WindowMain(HINSTANCE hInstance);
	~WindowMain();

	bool InitMainWindow();

	HWND GetWindowHandle();
	int Run();

	bool IsAppPaused() { return IsPaused; }
	void SetAppPaused(bool InAppPaused) { IsPaused = InAppPaused; }

private:

	HINSTANCE		ApplicationInstanceHandle = nullptr;
	HWND			MainWindowHandle = nullptr;
	std::wstring	MainWindowHandleCaption = L"d3d App";
	int				ClientWidth = 800;
	int				ClientHeight = 600;

	bool IsPaused = false;
};

