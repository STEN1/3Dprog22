#include "ThreadPool.h"

#include "Core/Logger.h"

ThreadPool::ThreadPool()
{
	auto numThreads = std::thread::hardware_concurrency();
	if (numThreads < 1)
		numThreads = 1;
	for (int i = 0; i < numThreads; i++)
		m_Workers.push_back(std::thread(&ThreadPool::Worker, this));
}

ThreadPool::~ThreadPool()
{
	m_IsWorking = false;
	for (auto& thread : m_Workers)
		thread.join();
}

void ThreadPool::WaitUntilFinished()
{
	while (true)
	{
		{
			std::scoped_lock<std::mutex> lock(m_WorkQueueMutex);
			if (m_WorkQueue.empty() && m_ThreadsWorking == 0)
				return;
		}
	}
}

void ThreadPool::PushWork(std::function<void()>&& func)
{
	std::scoped_lock<std::mutex> lock(m_WorkQueueMutex);
	m_WorkQueue.emplace_back(func);
}

void ThreadPool::Worker()
{
	while (m_IsWorking)
	{
		std::function<void()> f;
		bool hasWork = false;
		{
			m_WorkQueueMutex.lock();
			if (m_WorkQueue.empty())
			{
				m_WorkQueueMutex.unlock();
				std::this_thread::sleep_for(std::chrono::microseconds(10));
			}
			else
			{
				m_ThreadsWorking += 1;
				hasWork = true;
				f = m_WorkQueue.back();
				m_WorkQueue.pop_back();
				m_WorkQueueMutex.unlock();
			}
		}
		if (hasWork)
		{
			f();
			m_ThreadsWorking -= 1;
		}
	}
}
