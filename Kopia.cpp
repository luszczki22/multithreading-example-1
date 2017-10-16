//by £uszczki Marcin 2017 | C++/11	multithreading	mutex
//Program creates 1D table and then concurently performs calculations on that table (with two threads), meanwhile displays it(in other thread). This simple example of managing acess to shared resource with use of mutex and thread sleep

#include "stdafx.h"
#include "iostream"
#include "thread"
#include "mutex"
#include <chrono>
#include <atomic>

//shared variable 
volatile int counter = 0;

std::mutex mtx;
std::atomic<int> th1Live = true;
std::atomic<int> thdFlag = false;

using namespace std;

void compute2(int *t, int time)
{
	//function changes contents of shared table
	bool mutexLocked = false;
	for (int i = 0; i<50; ++i)
	{
		if (!mutexLocked) {
			mtx.lock();
			mutexLocked = true;
		}
		
		// calculations performed in thread on shared table 
		int a = 1;
		for (int j = 0; j<50; ++j)
		{
			a = i + j;
			t[j] = a;

			std::cout << "Thread with calculations ended, time = %" << time << std::endl;
			//blocking of shared resource 
		}
		//evry tenth iteration function gos to sleep for desired time and unlocks mutex
		if (!((i + 1) % 10))
		{
			mtx.unlock();
			mutexLocked = false;
			std::cout << "//mtx unlock " << i << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(time));
			thdFlag = true;
		}
	}
	th1Live = false;
}

void display2(int *t)
{
	//function displays shared table
	while (th1Live)
	{
		if (mtx.try_lock()) {

			for (int j = 0; j < 50; ++j)
			{

				std::cout << "t[" << j << "] = " << t[j] << " ";
				cout << "" << std::endl;
			}
			mtx.unlock();
		}
		else
		{
			std::cout << "mtx not aquired -  display " << std::endl;
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

int main(int argc, char* argv[])
{
	int *tab2 = new int[50];	//shared table between threads

	for (int i = 0; i<50; i++)	//loop fills table
	{
		tab2[i] = i;
	}
	//creation of threads
	std::thread thd(compute2, tab2, 3500);
	std::thread thd2(compute2, tab2, 3600);
	std::thread thd5(display2, tab2);

	//joining threads
	thd.join();
	thd2.join();
	thd5.join();

	delete[] tab2;
	 
	getchar();
	return 0;
}


