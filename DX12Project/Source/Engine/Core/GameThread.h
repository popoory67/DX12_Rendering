#pragma once
#include <functional>
#include <memory>
#include <mutex>
#include <condition_variable>

namespace GameThread
{
	void StartGameThread(std::shared_ptr<std::mutex> InMutex, std::shared_ptr<std::condition_variable> InCondition);
	void StopGameThread();
};