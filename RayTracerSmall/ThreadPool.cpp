#include "ThreadPool.h"

ThreadPool::ThreadPool(unsigned int numThreads, std::mutex* main_mutex)
{
	threadCount = numThreads;
	mainMutex = main_mutex;
	
	tasks = queue<std::function<void()>>();

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
					Lock();
					//wait->lock();
					if (!tasks.empty())
					{
						if (stopping)
						{
							ReleaseLock();
							//wait->unlock();
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
						//wait->unlock();
				}
			}));
	}
}

ThreadPool::~ThreadPool()
{
	stopping = true;
	delete(mainMutex);
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