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

	int GetSize() { return threadCount; }

	void Enqueue(std::function<void()> task);

	queue<std::function<void()>> GetTasks() { return tasks; }

	void WaitUntilCompleted();

	void Lock();
	void ReleaseLock();

private:
	vector<std::thread> threads;
	queue<std::function<void()>> tasks;
	int tasksRemaining = 0;
	int threadCount;

	bool stopping = false;

	std::mutex(wait);
	std::mutex* mainMutex;
	std::condition_variable cv;

	std::unique_lock<std::mutex> lock;
};