#ifndef THREAD_SAFE_QUEUE
#define THREAD_SAFE_QUEUE

//Адаменко Екатерина, 492

#include <queue>
#include <mutex>
#include <deque>
#include <condition_variable>
#include <atomic>
#include <exception>

template <typename T, class Container = std::deque<T>>
class thread_safe_queue {
public:
	explicit thread_safe_queue(std::size_t capacity) : capacity(capacity), available(true) {}
	thread_safe_queue(const thread_safe_queue &) = delete;
	void enqueue(T && item);
	void pop(T & item);
	void shutdown() {
		available.store(false);
	}
private:
	Container q;
	const std::size_t capacity;
	std::mutex queue_mutex;
	std::condition_variable not_empty_cv;
	std::condition_variable not_full_cv;
	std::atomic<bool> available;
};

template <typename T, class Container>
void thread_safe_queue<T, Container>::enqueue(T && item) {
	std::unique_lock<std::mutex> lck2(queue_mutex);
	if (!available.load())
		throw std::exception();
	if (q.size() == capacity) { 
		not_full_cv.wait(lck2, [this]() {return q.size() != capacity; });
	}
	q.push_back(std::move(item));
	not_empty_cv.notify_one();
}

template <typename T, class Container>
void thread_safe_queue<T, Container>::pop(T & item) {
	std::unique_lock<std::mutex> lck1(queue_mutex);
	if (!available.load() && q.empty())
		throw std::exception();
	if (q.empty()) {
		not_empty_cv.wait(lck1, [this]() {return !q.empty(); });
	}
	item = std::move(q.front());
	q.pop_front();
	not_full_cv.notify_one();
}

#endif