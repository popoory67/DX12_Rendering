#pragma once

#include <Windows.h>
#include <WindowsX.h>
#include <windows.h>
#include "Application.h"

class WindowMain : public Application
{
	using Parent = Application;

public:

	WindowMain(HINSTANCE hInstance);
	~WindowMain();

	virtual bool Init();
	virtual int Run();

	HWND GetWindowHandle();

	bool IsAppPaused() { return IsPaused; }
	void SetAppPaused(bool InAppPaused) { IsPaused = InAppPaused; }

private:
	bool InitMainWindow();

private:

	HINSTANCE		ApplicationInstanceHandle = nullptr;
	HWND			MainWindowHandle = nullptr;
	std::wstring	MainWindowHandleCaption = L"d3d App";
	int				ClientWidth = 800;
	int				ClientHeight = 600;

	bool IsPaused = false;
};

