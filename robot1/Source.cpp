#include <iostream>
#include <condition_variable>
#include <thread>
#include <string>
#include <mutex>
#include <chrono>

//робот на условных переменных:

void step(std::string s) {
	std::cout << s << std::endl;
}

int main() {
	std::condition_variable left, right;
	bool began_l = false, began_r = false;
	std::mutex m;
	std::thread th_l([&left, &right, &began_l, &m]() {
		std::unique_lock<std::mutex> lock(m);
		for (int i = 0; i < 10; ++i) {
			if (began_l)
				left.wait(lock);
			else
				began_l = true;
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			step("left");
			right.notify_one();
		}
	});
	std::thread th_r([&left, &right, &began_r, &m]() {
		std::unique_lock<std::mutex> lock(m);
		for (int i = 0; i < 10; ++i) {
			if (began_r)
				right.wait(lock);
			else
				began_r = true;
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			step("right");
			left.notify_one();
		}
	});
	th_l.join();
	th_r.join();
}

//закомментированный код - робот на семафорах

/*
#include <iostream>
#include <chrono>
#include <string>
#include <thread>
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

void step(std::string s) {
	std::cout << s << std::endl;
}

int main() {
	semaphore s_l(1), s_r(0);
	std::thread th_l([&s_l, &s_r]() {
		for (int i = 0; i < 10; ++i) {
			s_l.wait();
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			step("left");
			s_r.signal();
		}
	});
	std::thread th_r([&s_l, &s_r]() {
		//std::this_thread::sleep_for(std::chrono::seconds(5));
		for (int i = 0; i < 10; ++i) {
			s_r.wait();
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			step("right");
			s_l.signal();
		}
	});
	th_l.join();
	th_r.join();
}
*/