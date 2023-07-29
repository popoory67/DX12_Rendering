#pragma once
#include <string>

class Application
{
public:
	Application();
	virtual ~Application();

	bool Initialize();
	virtual int Run() = 0;

	class GameTimer& GetTimer() const;

	static void* GetWindowHandle();

protected:
	std::wstring Caption;
	int ClientWidth;
	int ClientHeight;

	bool IsPaused = false;

	static void* MainWindowHandle;
};