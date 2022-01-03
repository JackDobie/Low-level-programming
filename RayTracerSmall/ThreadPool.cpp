#include "ThreadPool.h"
#include <sstream>
#ifndef _WIN32
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif // !_WIN32

ThreadPool::ThreadPool(unsigned int numThreads, std::mutex* main_mutex)
{
#ifdef _WIN32
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
					{
						break;
					}
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
							ReleaseLock();
							task();
							if (--tasksRemaining == 0)
							{
								cv.notify_one(); // unblock main thread when all tasks are done
							}
							//ReleaseLock();
						}
					}
					else
						ReleaseLock();
				}
			}));
	}
#endif
}

ThreadPool::~ThreadPool()
{
#ifdef _WIN32
	stopping = true;
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
    int status;
    while(wait(&status) != -1){}
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
	if (newFork == 0) // child
	{
        if (task)
        {
            task();
        }
		exit(0);
	}
	else if (newFork < 0) // fail
	{
		std::cout << "Error: Could not create fork!\n";
	}
}
#endif // _WIN32
