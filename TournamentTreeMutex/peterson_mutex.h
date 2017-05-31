#ifndef PETERSON_MUTEX
#define PETERSON_MUTEX

#include <thread>
#include <atomic>
#include <array>

class peterson_mutex {
public:
	peterson_mutex();
	peterson_mutex(peterson_mutex &&) : victim(0) {}
	void lock(int t);
	void unlock(int t);
private:
	std::array<std::atomic<bool>, 2> want;
	std::atomic<int> victim;
};

#endif