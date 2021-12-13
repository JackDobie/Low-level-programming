#pragma once

#include <functional>
#include <vector>
#include <thread>
#include <queue>
#include <future>

using std::vector;
using std::queue;

class ThreadPool
{
public:
	ThreadPool(unsigned int numThreads);
	~ThreadPool();

	int GetSize() { return threadCount; }

	void Enqueue(std::function<void()> task);

	queue<std::function<void()>> GetTasks() { return tasks; }

	//template<class T>
	//auto Enqueue(T task)->std::future<decltype(task())>;

private:
	vector<std::thread> threads;
	queue<std::function<void()>> tasks;
	int threadCount;

	bool stopping = false;

	std::mutex* wait;
};