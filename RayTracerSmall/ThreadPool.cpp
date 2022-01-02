#include "ThreadPool.h"
#include <sstream>
#ifndef _WIN32
#include <unistd.h>
#endif // !_WIN32

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
//#else
//	for (int i = 0; i < numThreads; i++)
//	{
//		pid_t newThread = fork();
//
//        if (newThread == 0)
//		{
//			threads.emplace_back(newThread);
//			while (true)
//			{
//				if (stopping)
//				{
//					ReleaseLock();
//					break;
//				}
//				if (!tasks.empty())
//				{
//                    std::cout << "Tasks not empty!\n";
//					Lock();
//					if (stopping)
//					{
//						ReleaseLock();
//						break;
//					}
//					std::function<void()> task = tasks.front();
//					tasks.pop();
//
//					if (task)
//					{
//						task();
//						if (--tasksRemaining == 0)
//						{
//							cv.notify_one(); // unblock main thread when all tasks are done
//						}
//						ReleaseLock();
//					}
//				}
//				else
//				{
//                    std::cout << "Tasks empty!\n";
//				}
//			}
//			_exit(0);
//		}
//		else if (newThread < 0)
//		{
//			std::stringstream msg;
//			msg << "Error: Could not create fork!" << std::endl;
//			std::cout << msg.str();
//		}
//	}
#endif
}

ThreadPool::~ThreadPool()
{
	stopping = true;
#ifdef _WIN32
	for (int i = 0; i < threadCount; i++)
	{
		threads[i].join();
	}
#endif // _WIN32
}

void ThreadPool::Enqueue(std::function<void()> task)
{
#ifdef _WIN32
	tasks.push(task);
	tasksRemaining++;
#else
	MakeForks(task);
#endif // _WIN32
}

void ThreadPool::WaitUntilCompleted()
{
#ifdef _WIN32
	std::unique_lock<std::mutex> lock(*mainMutex);
	cv.wait(lock);
#else
	while (!forks.empty())
	{
		// wait for forks to be done
	}
#endif // _WIN32
}

#ifdef _WIN32
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
#else
void ThreadPool::MakeForks(std::function<void()> task)
{
	pid_t newFork = fork();
	if (newFork == 0)
	{
		forks.emplace_back(newFork);
		if (!tasks.empty())
		{
			//std::function<void()> task = tasks.front();
			tasks.pop();

			if (task)
			{
				task();
			}
		}
		forks.swap(std::remove(forks.begin(), forks.end(), newFork), forks.end());
		forks.pop_back();
		_exit(0);
	}
	else if (newThread < 0)
	{
		std::cout << "Error: Could not create fork!\n";
	}
}
#endif // _WIN32
