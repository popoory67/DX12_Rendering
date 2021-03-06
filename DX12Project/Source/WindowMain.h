#pragma once

#include <Windows.h>
#include <WindowsX.h>
#include <windows.h>
#include "Application.h"

class WindowMain : public Application
{
	using Parent = Application;

public:
	WindowMain() = delete;
	WindowMain(HINSTANCE hInstance);
	~WindowMain();

	virtual bool Init();
	virtual int Run();

	bool IsAppPaused() { return IsPaused; }
	void SetAppPaused(bool InAppPaused) { IsPaused = InAppPaused; }

private:
	bool InitMainWindow();
};

