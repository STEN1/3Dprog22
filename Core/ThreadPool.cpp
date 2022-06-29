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
		m_WorkQueueMutex.lock();
		if (m_WorkQueue.empty() && m_ThreadsWorking.load(std::memory_order::memory_order_acquire) == 0)
		{
			m_WorkQueueMutex.unlock();
			break;
		}
		m_WorkQueueMutex.unlock();
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
				std::this_thread::sleep_for(std::chrono::microseconds(100));
			}
			else
			{
				m_ThreadsWorking.fetch_add(1, std::memory_order::memory_order_release);
				hasWork = true;
				f = std::move(m_WorkQueue.back());
				m_WorkQueue.pop_back();
				m_WorkQueueMutex.unlock();
			}
		}
		if (hasWork)
		{
			f();
			m_ThreadsWorking.fetch_sub(1, std::memory_order::memory_order_release);
		}
	}
}
