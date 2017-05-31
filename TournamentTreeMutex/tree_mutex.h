#ifndef TREE_MUTEX
#define TREE_MUTEX

#include "peterson_mutex.h"
#include <vector>

class tree_mutex {
public:
	tree_mutex(std::size_t num_threads);
	void lock(std::size_t thread_index);
	void unlock(std::size_t thread_index);
private:
	std::vector<peterson_mutex> tree;
	std::vector<unsigned int> logs;
	std::size_t parent_index(std::size_t thread_index);
	bool is_left_child(std::size_t thread_index);
	bool is_right_child(std::size_t thread_index);
	std::size_t left_child(std::size_t thread_index);
	std::size_t right_child(std::size_t thread_index);
};

#endif