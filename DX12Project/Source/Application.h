#pragma once

class Application
{
public:
	Application() = delete;
	Application(HINSTANCE hInstance);
	virtual ~Application();

	static HWND GetWindowHandle();

	virtual bool Init();
	virtual int Run();

	class GameTimer& GetTimer() const;

protected:
	static HWND			MainWindowHandle;

	HINSTANCE		ApplicationInstanceHandle = nullptr;
	std::wstring	MainWindowHandleCaption = L"d3d App";
	int				ClientWidth = 800;
	int				ClientHeight = 600;

	bool IsPaused = false;

};