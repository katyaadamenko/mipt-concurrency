#include "tree_mutex.h"

static unsigned int floor_log(unsigned int number) {
	if (number == 1)
		return 0;
	return floor_log(number / 2) + 1;
}

inline std::size_t tree_mutex::parent_index(std::size_t thread_index) {
	return (thread_index - 1) / 2;
}

inline bool tree_mutex::is_left_child(std::size_t thread_index) {
	return (thread_index & 1) == 1;
}

inline bool tree_mutex::is_right_child(std::size_t thread_index) {
	return (thread_index & 1) == 0;
}

inline std::size_t tree_mutex::left_child(std::size_t thread_index) {
	return 2 * thread_index + 1;
}

inline std::size_t tree_mutex::right_child(std::size_t thread_index) {
	return 2 * thread_index + 2;
}

tree_mutex::tree_mutex(std::size_t num_threads) {
	unsigned int log = floor_log(num_threads);
	size_t num_leaves;
	num_leaves = ((1 << log) == num_threads) ? (1 << log) : (1 << (log + 1));
	size_t tree_size = (num_leaves != 1) ? num_leaves - 1 : 1;
	tree.resize(tree_size);
	logs.resize(2 * tree_size + 1);
	for (std::size_t i = 0; i < logs.size(); ++i)
		logs[i] = floor_log(i + 1);
}

void tree_mutex::lock(std::size_t thread_index) {
	size_t num_leaves = tree.size() + 1;
	thread_index += num_leaves - 1;
	int index_in_mutex;
	while (thread_index > 0) {
		index_in_mutex = is_left_child(thread_index) ? 0 : 1;
		thread_index = parent_index(thread_index);
		tree[thread_index].lock(index_in_mutex);
	}
}

void tree_mutex::unlock(std::size_t thread_index) {
	std::size_t cur_index = 0;
	std::size_t index_in_mutex;
	std::size_t median_index;
	std::size_t num_elements = tree.size() + 1;
	while (cur_index < tree.size()) {
		std::size_t right = right_child(cur_index);
		median_index = (right - (1 << logs[right - 1]) + 1) * (num_elements / (1 << logs[right - 1]));
		index_in_mutex = (thread_index < median_index) ? 0 : 1;
		tree[cur_index].unlock(index_in_mutex);
		cur_index = (index_in_mutex == 0) ? left_child(cur_index) : right_child(cur_index);
	}
}
