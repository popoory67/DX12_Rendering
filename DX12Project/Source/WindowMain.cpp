#include "stdafx.h"
#include "WindowMain.h"
#include "InputManager.h"
#include "D3D12/D3DApp.h"

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return InputManager::GetInstance().MsgProc(hwnd, msg, wParam, lParam);
}

WindowMain::WindowMain(HINSTANCE hInstance)
	: ApplicationInstanceHandle(hInstance)
{

}

WindowMain::~WindowMain()
{
}

bool WindowMain::InitMainWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = ApplicationInstanceHandle;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"MainWnd";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, ClientWidth, ClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	MainWindowHandle = CreateWindow(L"MainWnd", MainWindowHandleCaption.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, ApplicationInstanceHandle, 0);

	if (!MainWindowHandle)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(MainWindowHandle, SW_SHOW);
	UpdateWindow(MainWindowHandle);

	// dx test
	D3DApp::GetInstance().Initialize(MainWindowHandle);

	return true;
}

HWND WindowMain::GetWindowHandle()
{
	return MainWindowHandle;
}


int WindowMain::Run()
{
	MSG msg = { 0 };

	GAME_TIMER.Reset();

	while (msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Otherwise, do animation/game stuff.
		else
		{
			GAME_TIMER.Tick();

			if (!IsPaused)
			{
				//CalculateFrameStats();
				D3DApp::GetInstance().Update(GAME_TIMER);
				D3DApp::GetInstance().Draw(GAME_TIMER);
			}
			else
			{
				Sleep(100);
			}
		}
	}

	return (int)msg.wParam;
}