#include "ThreadPool.h"
#include <sstream>

ThreadPool::ThreadPool(unsigned int numThreads, std::mutex* main_mutex)
{
	threadCount = numThreads;
	mainMutex = main_mutex;
	
	tasks = queue<std::function<void()>>();

#ifdef _WIN32
	for (int i = 0; i < numThreads; i++)
	{
		threads.emplace_back(std::thread([this]()
			{
				while (true)
				{
					if (stopping)
					{
						ReleaseLock();
						break;
					}
					if (!tasks.empty())
					{
						Lock();
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
							ReleaseLock();
						}
					}
				}
			}));
	}
#elif __linux__
	for (int i = 0; i < numThreads; i++)
	{
		pid_t newThread = vfork();
		if (newThread == 0)
		{
			threads.emplace_back(newThread);
			while (true)
			{
				if (stopping)
				{
					ReleaseLock();
					break;
				}
				if (!tasks.empty())
				{
					Lock();
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
						ReleaseLock();
					}
				}
			}
			_exit(0);
		}
		else if (newThread < 0)
		{
			std::stringstream msg;
			msg << "Error: Could not create fork!" << std::endl;
			std::cout << msg.str();
		}
	}
#endif
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