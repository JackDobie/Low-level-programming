#include "ThreadPool.h"
#include <sstream>
#ifndef _WIN32
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// this is used with pthread to execute a member function (threadfunc)
void* ExecThreadFunc(void* obj)
{
    return ((ThreadPool*)obj)->ThreadFunc();
}
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
            ThreadFunc();
            /*while (true)
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
            }*/
        }));
	}
#else
    if(LINUX_POOLING)
    {
        // initialise threads vector
        threads = vector<pthread_t>(numThreads);
        for (int i = 0; i < numThreads; i++)
        {
            // create pthreads with function execmemberfunction, which will call threadfunc
            pthread_create(&threads[i], NULL, &ExecThreadFunc, this);
        }
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
#else
    if(LINUX_POOLING)
    {
        stopping = true;
        for (int i = 0; i < threadCount; i++)
        {
            pthread_join(threads[i], NULL);
        }
    }
#endif // _WIN32
}

void ThreadPool::Enqueue(std::function<void()> task)
{
#ifdef _WIN32
    tasks.push(task);
    tasksRemaining++;
#else
    if(LINUX_POOLING)
    {
        tasks.push(task);
        tasksRemaining++;
    }
    else
    {
        MakeForks(task);
    }
#endif // _WIN32
}

void ThreadPool::WaitUntilCompleted()
{
#ifdef _WIN32
    std::unique_lock<std::mutex> lock(*mainMutex);
    cv.wait(lock);
#else
    if(LINUX_POOLING)
    {
        std::unique_lock<std::mutex> lock(*mainMutex);
        cv.wait(lock);
    }
	else
	{
        int status;
        while(wait(&status) != -1){}
	}
#endif // _WIN32
}

void ThreadPool::Lock()
{
	std::unique_lock<std::mutex> localLock(waitMutex);
	lock = std::move(localLock);
}

void ThreadPool::ReleaseLock()
{
	std::unique_lock<std::mutex> localLock = std::move(lock);
	localLock.unlock();
}

void *ThreadPool::ThreadFunc()
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
}

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
