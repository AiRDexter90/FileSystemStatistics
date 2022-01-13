#include <iostream>
#include <conio.h>
#include <thread>
#include <mutex>
#include <chrono>

/*
	
    std::cout << "f(42) = " << fibonacci(42) << '\n';
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
*/

using namespace std;
typedef unsigned int  uint;

uint hc = thread::hardware_concurrency();				//how many concurent threads would be available

mutex m;		//test mutex lock

/*Every thread will be counting numbers up to 'liczba' value and introduce itself*/
void licz(int liczba,int watek)
{
	for (int i = 0; i < liczba; i++)
	{
		m.lock();	
		cout << "W: " << watek << "    Liczba: " << i << endl;
		m.unlock();
	}
}

int main()
{
	auto start1 = chrono::steady_clock::now();
	thread w1(licz, 100, 1);
	auto start2 = std::chrono::steady_clock::now();
	thread w2(licz, 1000, 2);
	w1.join();
	auto end1 = std::chrono::steady_clock::now();
	w2.join();
	auto end2 = std::chrono::steady_clock::now();
	cout << "Hardware concurrency: "<<hc << endl;
	std::chrono::duration<double> elapsed_seconds1 = end1 - start1;
	std::chrono::duration<double> elapsed_seconds2 = end2 - start2;
	std::cout << "elapsed time: " << elapsed_seconds1.count() << "s\n";
	std::cout << "elapsed time: " << elapsed_seconds2.count() << "s\n";
	return 0;
}