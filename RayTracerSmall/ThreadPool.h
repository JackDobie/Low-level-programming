#pragma once

#include <condition_variable>
#include <functional>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <iostream>
#include "Global.h"

using std::vector;
using std::queue;

class ThreadPool
{
public:
	ThreadPool(unsigned int numThreads, std::mutex* main_mutex);
	~ThreadPool();

#ifdef _WIN32
	int GetSize() { return threadCount; }

	void Lock();
	void ReleaseLock();
#endif // _WIN32

	void Enqueue(std::function<void()> task);

	queue<std::function<void()>> GetTasks() { return tasks; }

	void WaitUntilCompleted();

private:
#ifdef _WIN32
	vector<std::thread> threads;
	queue<std::function<void()>> tasks;
	int tasksRemaining = 0;
	int threadCount;

	std::mutex(wait);
	std::mutex* mainMutex;
	std::condition_variable cv;

	std::unique_lock<std::mutex> lock;
#else
	std::vector<pid_t> forks;
	void MakeForks(std::function<void()> task);
#endif
	bool stopping = false;
};
