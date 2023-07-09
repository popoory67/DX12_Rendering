#pragma once

class Application
{
public:
	Application();
	virtual ~Application();

	bool Initialize(std::shared_ptr<class RHI> InRenderInterface);
	virtual int Run() = 0;

	class GameTimer& GetTimer() const;

protected:
	std::wstring Caption;
	int ClientWidth;
	int ClientHeight;

	bool IsPaused = false;

	void* MainWindowHandle = nullptr;

private:
	std::unique_ptr<class SceneRenderer> Renderer; // 3D
	std::unique_ptr<class Viewport> ViewportRenderer; // 2D
};