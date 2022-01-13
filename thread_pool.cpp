#include "thread_pool.h"

void thread_pool::start(uint t_number)
{
	for (int i = 1; i <= t_number; i++)
	{
		cout <<i<< " started" << endl;
		vThreads.emplace_back(thread([=] {
			
			while (true)
			{
				Task task;
				
				//critical section
				{
					unique_lock<mutex> mlock{ M };		//lock M
					cond.wait(mlock, [=] {
						if (mStopping || !qTasks.empty()) return true;
						else false;
					});											//sleep thread until mStoppin = true;
					
					if (mStopping && qTasks.empty()) break;						//just break when destructor takes action
					else
					{
						task = move(qTasks.front());			//we have to move, because we are going to delete the function from the queue
						qTasks.pop();							//delete taken by the thread fun from queue
					}
				}
				//end of critical section
				
				task();//perform the task
			}//unlock mutex

		}));				//create so many threads with empty call
	}
}

void thread_pool::stop()
{
	{
		unique_lock<mutex> mlock{ M };//lock M
		mStopping = true;					//zakonczymy watki
	}//unlock M
	cond.notify_all();					//zezwol wszystkim na start
		
	for (auto &t : vThreads)
	{
		t.join();							//czekamy na zakonczenie poszczegolych
	}

}//unlock M

thread_pool::thread_pool(uint t_number)
{
	start(t_number);
}

thread_pool::~thread_pool()
{
	stop();
}

void thread_pool::enqueue(Task task)
{
	unique_lock<mutex> lock{ M };
	qTasks.push(move(task));			//move to avoid vanish of the lmbda function
}//here is release of lock
