#pragma once

#include <functional>
#include <vector>
#include <thread>
#include <queue>

using std::vector;
using std::queue;

class ThreadPool
{
public:
	ThreadPool(unsigned int numThreads);
	~ThreadPool();

	int GetSize() { return threadCount; }

	void Enqueue(std::function<void()> task);
private:
	void Init(unsigned int numThreads);

	vector<std::thread*> threads;
	queue<std::function<void()>> tasks;
	int threadCount;
};
