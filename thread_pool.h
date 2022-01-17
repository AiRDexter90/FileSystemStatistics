#pragma once
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <condition_variable>
#include <iostream>
#include <mutex>

using namespace std;
typedef unsigned int uint;
using Task = function<void()>;

//CLASS THREAD_POOL********************************************************************************
class thread_pool
{
private:
	bool mStopping = false;//flag to stop threads
	vector<thread> vThreads;//vector of threads
	mutex M;//to guard critical sections
	condition_variable cond;//decides when to bind a task to a thread
	queue<function<void()>> qTasks;//queue of tasks to do by idle threads

	
	void start(uint t_number);//activates given numbe of threads
	void stop();//deactivates threads
public:
	thread_pool(uint t_number = thread::hardware_concurrency()); //constructor
	~thread_pool();//destructor
	void enqueue(Task task);//add new tasks to the queue
};
