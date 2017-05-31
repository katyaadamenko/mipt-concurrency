#ifndef THREAD_POOL
#define THREAD_POOL

#include <functional>
#include <future>
#include <limits>
#include <vector>
#include "thread_safe_queue.h"

const std::size_t default_num_threads = 4;

//&thread_pool<ResultType>::

template <typename ResultType>
class thread_pool {
public:
	thread_pool() : workers(default_num_workers(), std::thread(&thread_pool<ResultType>::worker_func)), q(std::numeric_limits<std::size_t>::max()){
		//create_workers();

	}
	thread_pool(std::size_t num_threads) : workers(num_threads, std::thread(&thread_pool<ResultType>::worker_func)), q(std::numeric_limits<std::size_t>::max()) {
		//create_workers();
	}
	std::future<ResultType> submit(const std::function<ResultType()> & func);
	void shutdown() {
		q.shutdown();
	}
private:
	thread_safe_queue<std::pair<std::function<ResultType()>, std::promise<ResultType>>> q;
	//std::size_t num_workers;
	std::vector<std::thread> workers;
	//void create_workers();
	void worker_func();
	std::size_t default_num_workers() {
		return (std::thread::hardware_concurrency() > 0) ? std::thread::hardware_concurrency() : default_num_threads;
	}
};

/*template <typename ResultType>
void thread_pool<ResultType>::create_workers() {
	for (std::size_t i = 0; i < num_workers; ++i) {
		std::thread worker([this]() {
			try {
				while (true) {
					std::pair<std::function<ResultType()>, std::promise<ResultType>> new_task;
					this->pop(new_task);
					std::promise<ResultType> prms = new_task.second;
					prms.set_value(new_task.first());
				}
			}
			catch (out_of_order & ex) {}
		});
		worker.detach();
	}
}*/

template <typename ResultType>
void thread_pool<ResultType>::worker_func() {
	/*try {
		while (true) {
			std::pair<std::function<ResultType()>, std::promise<ResultType>> new_task;
			q.pop(new_task);
			//std::promise<ResultType> prms(std::move(new_task.second));
			//ResultType res = new_task.first();
			//new_task.second.set_value(new_task.first());
		}
	}
	catch (out_of_order &) {}*/
}

template <typename ResultType>
std::future<ResultType> thread_pool<ResultType>::submit(const std::function<ResultType()> & func) {
	std::promise<ResultType> prms;
	auto ftr = prms.get_future();
	q.enqueue(std::make_pair(func, std::move(prms)));
	return ftr;
}

#endif