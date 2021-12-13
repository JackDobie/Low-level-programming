#include "ThreadPool.h"
#include <iostream>

ThreadPool::ThreadPool(unsigned int numThreads)
{
	threadCount = numThreads;
	
	tasks = queue<std::function<void()>>();

	wait = new std::mutex();

	for (int i = 0; i < numThreads; i++)
	{
		/*std::thread* newThread = new std::thread();
		threads.push_back(newThread);*/

		threads.emplace_back(std::thread([this]()
			{
				while (true)
				{
					if (stopping)
						break;
					wait->lock();
					if (!tasks.empty())
					{
						auto task = tasks.front();
						if (task)
						{
							task();
						}
						tasks.pop();
					}
					wait->unlock();
				}
			}));
	}
}

ThreadPool::~ThreadPool()
{
	stopping = true;
	delete(wait);
}

void ThreadPool::Enqueue(std::function<void()> task)
{
	tasks.push(task);
}

//template<class T>
//auto ThreadPool::Enqueue(T task) -> std::future<decltype(task())>
//{
//	auto wrapper = std::make_shared<std::packaged_task<decltype(task()) ()>>(std::move(task));
//
//	tasks.emplace([=]
//		{
//			(*wrapper)();
//		});
//	return wrapper->get_future();
//}