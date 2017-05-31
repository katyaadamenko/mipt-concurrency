#ifndef BARRIER
#define BARRIER

#include <mutex>
#include <condition_variable>
#include <thread>

class barrier {
public:
	explicit barrier(std::size_t num_threads) : num_threads(num_threads), num_blocked_threads(0), era(0) {}
	void enter();
private:
	std::size_t num_threads;
	std::size_t num_blocked_threads;
	std::size_t era;
	std::mutex barrier_mutex;
	std::condition_variable all_in_barrier_cv;
	std::condition_variable new_era_cv;
};

void barrier::enter() {
	std::unique_lock<std::mutex> lock(barrier_mutex);
	if (era != 0)
		new_era_cv.wait(lock, [this]() {return era == 0; });
	++num_blocked_threads;
	if (num_threads != num_blocked_threads)
		all_in_barrier_cv.wait(lock);
	else {
		era = 1;
		all_in_barrier_cv.notify_all();
	}
	--num_blocked_threads;
	if (num_blocked_threads == 0) {
		era = 0;
		new_era_cv.notify_all();
	}
}


#endif