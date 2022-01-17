#include "thread_pool.h"

//CONSTRUCTOR-----------------------------------------------------------
thread_pool::thread_pool(uint t_number)
{
	start(t_number);
}

//DESTRUCTOR----------------------------------------------------------
thread_pool::~thread_pool()
{
	stop();
}

//METHODS---------------------------------------------------------------------------------
//activate thread pool by launching a given number of threads-------------------------------
void thread_pool::start(uint t_number)
{
	for (uint i = 1; i <= t_number; i++)
	{
		//create vector of active threads
		vThreads.emplace_back(thread([=] {

			while (true)
			{
				Task task;//task to be done
				
				//start of the critical section - must be guarded by mutex
				{
					unique_lock<mutex> mlock{ M };		//lock M
					cond.wait(mlock, [=] { return mStopping || !qTasks.empty(); });		//idle cycles as long as no task to do ;

					if (mStopping && qTasks.empty()) break; //deactivate all threads in the pool
					task = move(qTasks.front());			//we have to move, because we are going to delete the function from the queue
					qTasks.pop();							//delete taken by the thread fun from the queue
				}
				//end of critical section - mutex unlocked
				task();//perform the task
			}
		}));
	}
}

//deactivate thread pool-------------------------------------------------------------------------
void thread_pool::stop()
{
	{
		unique_lock<mutex> mlock{ M };//lock M
		mStopping = true; //set the flag to stop
	}//unlock M
	cond.notify_all();//release all

	for (auto& t : vThreads)
	{
		t.join();//wait for all threads to stop
	}
}//unlock M

//enqueue of waiting tasks-----------------------------------------------------------------------------
void thread_pool::enqueue(Task task)
{
	unique_lock<mutex> lock{ M };//lock M
	qTasks.push(move(task));			
}//here is release of lock
