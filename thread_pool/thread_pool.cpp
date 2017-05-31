#include <cstdlib>
#include <functional>
#include <iostream>
#include "thread_pool.h"

bool isPrime(int number) {
	bool isPrime = true;
	int bound = (int)sqrt(number);
	for (int divider = 2; divider <= bound && isPrime; ++divider) {
		if (number % divider == 0)
			isPrime = false;
	}
	return isPrime;
}

int main() {
	thread_pool<bool> tasks(5);
	for (int i = 0; i < 10; ++i) {
		int number = rand() % 100;
		auto ftr = tasks.submit(std::bind(isPrime, number));
		std::cout << number << (ftr.get() ? "is prime" : "is not prime") << std::endl;
	}
	tasks.shutdown();
	/*try {
		tasks.submit(std::bind(isPrime, 2));
	}
	catch(out_of_order & ex) {
		std::cout << "Impossible" << std::endl;
	}*/
	return 0;
}

//std::function<int()>(