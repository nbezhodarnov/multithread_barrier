#include <iostream>
#include <condition_variable>
#include <thread>
#include <chrono>

class barrier {

public:
	barrier(int count): thread_count(count), counter(0), waiting_threads(0) {
	}

	void wait() {
		std::unique_lock<std::mutex> lock(mtx);
		++counter;
		++waiting_threads;
		cv.wait(lock, [&]{
			return counter >= thread_count;
		});
		cv.notify_one();
		--waiting_threads;
		if(waiting_threads == 0) {
			counter = 0;
		}
	}

private:
		std::mutex mtx;
		std::condition_variable cv;
		int counter;
		int waiting_threads;
		int thread_count;
};

barrier barrier_var(3);

std::mutex cout_mtx;

void output(const char *string) {
	std::unique_lock<std::mutex> lock(cout_mtx);
	std::cout << string;
}


void func1() {
	output("Starting 3 seconds timer...\n");
	std::this_thread::sleep_for(std::chrono::seconds(3));
	barrier_var.wait();
	output("3 seconds have passed.\n");
}

void func2() {
	output("Waiting for synchronize all threads...\n");
	barrier_var.wait();
	output("All threads have been synchronized.\n");
}

int main() {
	std::thread t1(func1);  
	std::thread t2(func2);
	std::thread t3(func2);
	t1.join();
	t2.join();
	t3.join();
}
