#pragma once

class Application
{
public:

	Application();
	virtual ~Application();

	virtual bool Init();
	virtual int Run();

	class GameTimer& GetTimer() const;

private:

};