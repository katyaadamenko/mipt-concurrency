#ifndef SPSC_RING_BUFFER
#define SPSC_RING_BUFFER

#include <vector>
#include <atomic>
#include <future>
#include <iostream>

template <class T>
class spsc_ring_buffer {
public:
	explicit spsc_ring_buffer(std::size_t size) : data(size + 1), capacity(size + 1) {}
	bool enqueue(T v);
	bool dequeue(T & v);
private:
	std::vector<T> data;
	std::atomic<std::size_t> head{ 0 }, tail{ 0 };
	const std::size_t capacity;
};

template <class T>
bool spsc_ring_buffer<T>::enqueue(T v) {
	size_t curr_tail = tail.load(std::memory_order_relaxed);
	size_t curr_head = head.load(std::memory_order_acquire);
	if ((curr_tail + 1) % capacity == curr_head)
		return false;
	data[curr_tail] = std::move(v);
	tail.store((curr_tail + 1) % capacity, std::memory_order_release);
	return true;
}

template <class T>
bool spsc_ring_buffer<T>::dequeue(T & v) {
	size_t curr_head = head.load(std::memory_order_relaxed);
	size_t curr_tail = tail.load(std::memory_order_acquire);
	if (curr_head == curr_tail)
		return false;
	v = data[curr_head];
	head.store((curr_head + 1) % capacity, std::memory_order_release);
	return true;
}

#endif