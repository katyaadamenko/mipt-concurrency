#include <future>
#include <iostream>
#include "spsc_ring_buffer.h"

uint64_t producer_work_loop(spsc_ring_buffer<int> & buf) {
	uint64_t sum = 0;
	int n = 10000000;
	for (int i = 0; i < n; ++i) {
		while (!buf.enqueue(i + 1)) {
			std::this_thread::yield();
		}
		sum += (i + 1);
	}
	return sum;
}

uint64_t consumer_work_loop(spsc_ring_buffer<int> & buf) {
	uint64_t sum = 0;
	int n = 10000000;
	for (int i = 0; i < n; ++i) {
		int new_element;
		while (!buf.dequeue(new_element)) {
			std::this_thread::yield();
		}
		sum += new_element;
	}
	return sum;
}

int main() {
	spsc_ring_buffer<int> buf(1024);
	std::future<uint64_t> produced_sum = std::async(producer_work_loop, std::ref(buf));
	std::future<uint64_t> consumed_sum = std::async(consumer_work_loop, std::ref(buf));
	std::cout << "Producer result: " << produced_sum.get() << std::endl;
	std::cout << "Consumer result: " << consumed_sum.get() << std::endl;
	return 0;
}