#include <iostream>
#include <chrono>
#include "barrier.h"

int main() {
	barrier b(3);
	std::thread th1([&b](){
		std::this_thread::sleep_for(std::chrono::seconds(1));
		for (int i = 0; i < 2; ++i) {
			b.enter();
			std::cout << "1" << std::endl;
		}
	});
	std::thread th2([&b](){
		std::this_thread::sleep_for(std::chrono::seconds(3));
		for (int i = 0; i < 2; ++i) {
			b.enter();
			std::cout << "2" << std::endl;
		}
	});
	std::thread th3([&b](){
		std::this_thread::sleep_for(std::chrono::seconds(5));
		for (int i = 0; i < 2; ++i) {
			b.enter();
			std::cout << "3" << std::endl;
		}
	});
	th1.join();
	th2.join();
	th3.join();
	return 0;
}
