#include <iostream>
#include "lock_free_queue.h"

/*template <typename T>
void lock_free_queue<T>::chain_pending_nodes(node * nodes) {
	node * last = nodes;
	while (node * const next = last->next) {
		last = next;
	}
	chain_pending_nodes(nodes, last);
}

template <typename T>
void lock_free_queue<T>::chain_pending_nodes(node * first, node * last) {
	(last->next).store(to_be_deleted.load());
	node * new_node = (last->next).load();
	while (!to_be_deleted.compare_exchange_strong(new_node, first)) {}
}

template <typename T>
void lock_free_queue<T>::chain_pending_node(node * n) {
	chain_pending_nodes(n, n);
}

*/
/*template <typename T>
void lock_free_queue<T>::try_reclaim(node * old_head) {
if (threads_in_deque.load() == 1) {
threads_in_deque.fetch_sub(1);
if (!threads_in_deque.load()) {
delete_nodes(nodes_to_delete);
}
delete old_head;
}
}*/

/*if (threads_in_deque.load() == 1) {
threads_in_deque.fetch_sub(1);
if (!threads_in_deque.load()) {
node * to_delete = to_be_deleted.load();
delete_nodes(to_delete, cur_head_next);
to_be_deleted.store(cur_head_next);
}
}*/



int main() {
	lock_free_queue<int> q;
	for (int i = 0; i < 100; ++i)
		q.enqueue(i);
	int item = 0;
	for (int i = 0; i < 100; ++i) {
		q.dequeue(item);
		std::cout << item << std::endl;
	}
	//if (q.dequeue(item))
	std::cout << "^^" << std::endl;
	return 0;
}