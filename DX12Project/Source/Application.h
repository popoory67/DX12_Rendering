#pragma once

class Application
{
public:

	Application();
	virtual ~Application();

	virtual bool Init();
	virtual int Run();

	GameTimer& GetTimer() const;

private:

};