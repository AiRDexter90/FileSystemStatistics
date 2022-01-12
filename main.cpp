#include <iostream>
#include <conio.h>
#include <thread>
#include <mutex>

using namespace std;

mutex m;

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
	thread w1(licz, 100, 1);
	thread w2(licz, 100, 2);
	w1.join();
	w2.join();
	return 0;
}