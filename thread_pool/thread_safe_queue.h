#ifndef THREAD_SAFE_QUEUE
#define THREAD_SAFE_QUEUE

#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <exception>

template <class T>
class thread_safe_queue {
public:
	thread_safe_queue(std::size_t capacity) : capacity(capacity), available(true) {}
	thread_safe_queue(const thread_safe_queue &) = delete;
	void enqueue(T & item);
	void pop(T & item);
	bool is_available()const {
		return available.load();
	}
	void shutdown() {
		available.store(false);
	}
private:
	std::queue<T> q;
	const std::size_t capacity;
	std::mutex queue_mutex;
	std::condition_variable not_empty_cv;
	std::condition_variable not_full_cv;
	std::atomic<bool> available;
};

class out_of_order {
public:
	out_of_order(const char * what_happened = "") : what_happend(what_happend) {}
	const char * what()const {
		return what_happend;
	}
private:
	const char * what_happend;
};

template <class T>
void thread_safe_queue<T>::enqueue(T & item) {
	std::unique_lock<std::mutex> lck2(queue_mutex);
	if (!is_available())
		throw out_of_order("Thread safed queue is not available");
	if (q.size() == capacity) {
		not_full_cv.wait(lck2, [this]() {return q.size() != capacity; });
	}
	q.push(item);
	not_empty_cv.notify_one();
}

template <class T>
void thread_safe_queue<T>::pop(T & item) {
	std::unique_lock<std::mutex> lck1(queue_mutex);
	if (!is_available() && q.empty())
		throw out_of_order("Thread safed queue is not available");
	if (q.empty()) {
		not_empty_cv.wait(lck1, [this]() {return !q.empty(); });
	}
	item = std::move(q.front());
	q.pop();
	not_full_cv.notify_one();
}

#endif