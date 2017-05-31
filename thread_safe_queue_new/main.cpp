#include "thread_safe_queue.h"
#include <thread>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <future>


//задача: являются ли числа простыми
//producer генерирует случайные числа, consumer решает задачу, очередь хранит сами числа

int main() {
	thread_safe_queue<int> q(3);
	std::mutex printing_mutex;
	std::thread producer([&q]() {
	try {
	while (true)
	q.enqueue(rand() % 100);
	}
	catch (...) {}
	});
	producer.detach();
	std::atomic<int> numberOfConsumers;
	for (int i = 0; i < 4; ++i) {
	std::thread consumer([&q, &printing_mutex, &numberOfConsumers]() {
	try {
	while (true) {
	bool isPrime = true;
	int number;
	q.pop(number);
	int bound = (int)sqrt(number);
	for (int divider = 2; divider <= bound && isPrime; ++divider) {
	if (number % divider == 0)
	isPrime = false;
	}
	std::lock_guard<std::mutex> lck(printing_mutex);
	std::cout << number << (isPrime ? " is prime" : " is not prime") << std::endl;
	}
	}
	catch (...) {
	std::cout << "end!!" << std::endl;
	numberOfConsumers.fetch_sub(1);
	}
	});
	numberOfConsumers.fetch_add(1);
	consumer.detach();
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	q.shutdown();
	while (numberOfConsumers.load() > 0) {}
	/*thread_safe_queue<std::promise<int>> tasks(3);
	for (int i = 0; i < 4; ++i) {
		std::thread consumer([&tasks, &printing_mutex, &numberOfConsumers]() {
			try {
				while (true) {
					std::promise<int> res;
					tasks.pop(res);
					std::lock_guard<std::mutex> lck(printing_mutex);
					std::cout << "OK" << std::endl;
				}
			}
			catch (std::exception &) {
				std::cout << "end!!" << std::endl;
				numberOfConsumers.fetch_sub(1);
			}
		});
		numberOfConsumers.fetch_add(1);
		consumer.detach();
	}
	for (int i = 0; i < 4; ++i) {
		std::promise<int> prms;

		auto ftr = prms.get_future();
		try {
			tasks.enqueue(std::move(prms));
		}
		catch (std::exception &) {}
	}
	tasks.shutdown();
	while (numberOfConsumers.load() > 0) {
	}
	std::cout << "end!!" << std::endl;*/
	return 0;
}