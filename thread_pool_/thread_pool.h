#ifndef THREAD_POOL
#define THREAD_POOL

//Адаменко Екатерина, 492

#include <functional>
#include <future>
#include <vector>
#include <thread>
#include <exception>
#include <limits>
#include <algorithm>
#include <cassert>
#include <queue>
#include <mutex>
#include <deque>
#include <condition_variable>
#include <atomic>
#include <exception>

const std::size_t default_num_threads = 4;

template <typename T, class Container = std::deque<T>>
class thread_safe_queue {
public:
	explicit thread_safe_queue(std::size_t capacity) : capacity(capacity), available(true) {}
	thread_safe_queue(const thread_safe_queue &) = delete;
	void enqueue(T && item);
	void pop(T & item);
	void shutdown() {
		std::unique_lock<std::mutex> lck(queue_mutex);
		available.store(false);
		not_empty_cv.notify_all();
		not_full_cv.notify_all();
	}
	bool is_available() {
		return available.load();
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
	if (!available.load()) {
		throw std::exception();
	}
	if (q.size() == capacity) {
		not_full_cv.wait(lck2, [this]() {
			if (!is_available())
				throw std::exception();
			return q.size() != capacity; });
	}
	if (!available.load()) {
		throw std::exception();
	}
	q.push_back(std::move(item));
	not_empty_cv.notify_one();
}

template <typename T, class Container>
void thread_safe_queue<T, Container>::pop(T & item) {
	std::unique_lock<std::mutex> lck1(queue_mutex);
	if (!available.load() && q.empty()) {
		throw std::exception();
	}
	if (q.empty()) {
		not_empty_cv.wait(lck1, [this]() {
			if (!is_available() && q.empty())
				throw std::exception();
			return !q.empty(); });
	}
	if (!available.load() && q.empty()){
		throw std::exception();
	}
	item = std::move(q.front());
	q.pop_front();
	not_full_cv.notify_one();
}

template <typename ResultType>
class thread_pool {
public:
	thread_pool(std::size_t num_threads = default_num_workers()) : q(std::numeric_limits<std::size_t>::max()) {
		for (size_t i = 0; i < num_threads; ++i)
			workers.emplace_back(std::thread(std::bind(&thread_pool<ResultType>::worker_function, this)));
	}
	thread_pool() : thread_pool(default_num_workers()) {}
	std::future<ResultType> submit(const std::function<ResultType()> & func);
	void shutdown();
private:
	thread_safe_queue<std::pair<std::function<ResultType()>, std::promise<ResultType>>> q;
	std::vector<std::thread> workers;
	void worker_function();
	static std::size_t default_num_workers() {
		return (std::thread::hardware_concurrency() > 0) ? std::thread::hardware_concurrency() : default_num_threads;
	}
};

template <typename ResultType>
void thread_pool<ResultType>::worker_function() {
	try {
		while (true) {
			std::pair<std::function<ResultType()>, std::promise<ResultType>> newTask;
			q.pop(newTask);
			try{
				newTask.second.set_value(newTask.first());
			}
			catch (std::exception &) {
				newTask.second.set_exception(std::current_exception());
			}

		}
	}
	catch (std::exception &) {
		return;
	}
}

template <typename ResultType>
std::future<ResultType> thread_pool<ResultType>::submit(const std::function<ResultType()> & func) {
	std::promise<ResultType> prms;
	auto ftr = prms.get_future();
	q.enqueue(std::make_pair(func, std::move(prms)));
	return move(ftr);
}

template <typename ResultType>
void thread_pool<ResultType>::shutdown() {
	q.shutdown();
	for (auto it = workers.begin(); it != workers.end(); ++it) {
		if (it->joinable()) {
			it->join();
		}
	}
}

#endif