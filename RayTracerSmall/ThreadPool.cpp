#include "ThreadPool.h"

ThreadPool::ThreadPool(unsigned int numThreads)
{
	threadCount = numThreads;
	Init(numThreads);
}

ThreadPool::~ThreadPool()
{
}

void ThreadPool::Enqueue(std::function<void()> task)
{
	tasks.push(task);
}

void ThreadPool::Init(unsigned int numThreads)
{
	for (int i = 0; i < numThreads; i++)
	{
		/*std::thread* newThread = new std::thread();
		threads.push_back(newThread);*/

		threads.emplace_back(std::thread([this]()
			{
				while (true)
				{
					auto task = tasks.front();
					if (task)
					{
						task();
					}
					tasks.pop();
				}
			}));
	}
}
