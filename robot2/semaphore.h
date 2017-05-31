#ifndef SEMAPHORE
#define SEMAPHORE

#include <condition_variable>
#include <mutex>

class semaphore {
public:
	semaphore(int initial_value) : counter(initial_value) {}
	void wait();
	void signal();
private:
	int counter;
	std::mutex mtx;
	std::condition_variable signal_cv;
};

void semaphore::wait() {
	std::unique_lock<std::mutex> lock(mtx);
	if (counter == 0)
		signal_cv.wait(lock, [this]() {return counter != 0; });
	--counter;
}

void semaphore::signal() {
	std::unique_lock<std::mutex> lock(mtx);
	++counter;
	if (counter == 1)
		signal_cv.notify_one();
}

#endif