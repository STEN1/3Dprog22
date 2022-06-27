#pragma once
#include <vector>
#include <thread>
#include <mutex>
#include <functional>

class ThreadPool
{
public:
	ThreadPool();
	~ThreadPool();
	void WaitUntilFinished();
	void PushWork(std::function<void()>&& func);
private:
	void Worker();
	std::vector<std::thread> m_Workers;
	std::vector<std::function<void()>> m_WorkQueue;
	std::mutex m_WorkQueueMutex;
	std::atomic<int> m_ThreadsWorking = 0;
	std::atomic<bool> m_IsWorking = true;
};
