#pragma once
#include <string>
#include <memory>

class Application
{
public:
	Application();
	virtual ~Application();

	bool Initialize();
	virtual int Run() = 0;

	void ResetTimer() const;
	void TickTimer() const;

	static void* GetWindowHandle();

protected:
	std::wstring Caption;
	int ClientWidth;
	int ClientHeight;

	bool IsPaused = false;

	static void* MainWindowHandle;

private:
	std::unique_ptr<class GameTimer> Timer;
};