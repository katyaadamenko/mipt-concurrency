#include <iostream>
#include <chrono>
#include <string>
#include <thread>
#include "semaphore.h"

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