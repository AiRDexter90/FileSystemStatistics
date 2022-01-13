#include "thread_pool.h"

using namespace std;
typedef unsigned int uint;

thread_pool tp(2);

void fun1()
{
	for (int i = 1; i <= 100; i++)
	{
		cout << "fn1: " << i << endl;
	}
}

void fun2()
{
	for (int i = 1; i <= 101; i++)
	{
		cout << "fn2: " << "Kocham Justyne." << endl;
	}
}

int main()
{
	{
		tp.enqueue(fun1);
		tp.enqueue(fun2);
	}
	
	return 0;
}