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
    // add numthreads threads which loop through threadfunc
	for (int i = 0; i < numThreads; i++)
	{
		threads.emplace_back(std::thread([this]() { ThreadFunc(); }));
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
    // tell threads to escape while loop
    stopping = true;
    // wait for threads to finish
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
    // add task to queue and add 1 to count
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
        // if not pooling, do forks instead
        MakeForks(task);
    }
#endif // _WIN32
}

void ThreadPool::WaitUntilCompleted()
{
#ifdef _WIN32
    // create a condition variable that locks the main thread until a condition is met
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
        // wait for all forks to be done
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
    // loops until told to stop
    while (true)
    {
        if (stopping)
        {
            break;
        }
        Lock(); // only one thread allowed past this point
        if (!tasks.empty())
        {
            // check again if need to stop just before getting task, if so release the lock and escape the while loop
            if (stopping)
            {
                ReleaseLock();
                break;
            }

            // get the task at the front of the queue and remove it from queue
            std::function<void()> task = tasks.front();
            tasks.pop();

            if (task)
            {
                // execute task
                //ReleaseLock();
                task();
                if (--tasksRemaining == 0) // count down after each task
                {
                    cv.notify_one(); // unblock main thread when all tasks are done
                }
                //ReleaseLock();
            }
        }
        else
            ReleaseLock(); // if tasks are empty, release the lock to allow the others to continue
    }
    return nullptr;
}

#ifndef _WIN32
void ThreadPool::MakeForks(std::function<void()> task)
{
    // create new fork
    pid_t newFork = fork();
    if (newFork == 0) // this is the child process
    {
        if (task)
        {
            task();
        }
        exit(0);
    }
    else if (newFork < 0) // the fork failed
    {
        std::cout << "Error: Could not create fork!\n";
    }
}
#endif // !_WIN32
