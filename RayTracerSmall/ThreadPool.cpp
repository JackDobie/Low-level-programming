#include "ThreadPool.h"

ThreadPool::ThreadPool(unsigned int numThreads, std::mutex* main_mutex)
{
	threadCount = numThreads;
	mainMutex = main_mutex;
	
	tasks = queue<std::function<void()>>();

	for (int i = 0; i < numThreads; i++)
	{
		threads.emplace_back(std::thread([this]()
			{
				while (true)
				{
					if (stopping)
						break;
					Lock();
					if (!tasks.empty())
					{
						if (stopping)
						{
							ReleaseLock();
							break;
						}
						std::function<void()> task = tasks.front();
						tasks.pop();

						if (task)
						{
							task();
							if (--tasksRemaining == 0)
							{
								cv.notify_one(); // unblock main thread when all tasks are done
							}
						}
					}
					else
					{
						ReleaseLock();
					}
				}
			}));
	}
}

ThreadPool::~ThreadPool()
{
	stopping = true;
	for (int i = 0; i < threadCount; i++)
	{
		threads[i].join();
	}
}

void ThreadPool::Enqueue(std::function<void()> task)
{
	tasks.push(task);
	tasksRemaining++;
}

void ThreadPool::WaitUntilCompleted()
{
	std::unique_lock<std::mutex> lock(*mainMutex);
	cv.wait(lock);
}

void ThreadPool::Lock()
{
	std::unique_lock<std::mutex> localLock(wait);
	lock = std::move(localLock);
}

void ThreadPool::ReleaseLock()
{
	std::unique_lock<std::mutex> localLock = std::move(lock);
	localLock.unlock();
}