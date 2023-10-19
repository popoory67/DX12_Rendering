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

	static float GetWidth();
	static float GetHeight();

protected:
	std::wstring Caption;

	bool IsPaused = false;

	static void* MainWindowHandle;

	static float ClientWidth;
	static float ClientHeight;

private:
	std::unique_ptr<class GameTimer> Timer;
};