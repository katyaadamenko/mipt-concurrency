#include "peterson_mutex.h"

peterson_mutex::peterson_mutex() {
	victim.store(0);
	want[0].store(false);
	want[1].store(false);
}

void peterson_mutex::lock(int t) {
	want[t].store(true);
	victim.store(t);
	while (want[1 - t].load() && victim.load() == t) {
		std::this_thread::yield();
	}
}

void peterson_mutex::unlock(int t) {
	want[t].store(false);
}