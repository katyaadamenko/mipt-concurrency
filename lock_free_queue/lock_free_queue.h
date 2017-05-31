#ifndef LOCK_FREE_QUEUE
#define LOCK_FREE_QUEUE

#include <atomic>

template <typename T>
class lock_free_queue {
public:
	lock_free_queue(){
		node * new_node = new node;
		head.store(new_node);
		tail.store(head.load());
		to_be_deleted.store(head.load());
	}
	~lock_free_queue() {
		node * to_delete = to_be_deleted.load();
		delete_nodes(to_delete, nullptr);
	}
	void enqueue(T item);
	bool dequeue(T & item);
private:
	struct node {
		T value;
		std::atomic<node *> next;
		node(T value = T()) : value(value), next(nullptr) {}
	};
	std::atomic<node *> head;
	std::atomic<node *> tail;
	std::atomic<node *> to_be_deleted;
	std::atomic<size_t> threads_in_deque{ 0 };
	void delete_nodes(node * nodes, node * last);
};

template <typename T>
void lock_free_queue<T>::delete_nodes(node * nodes, node * last) {
	while (nodes != last) {
		node * next = nodes->next;
		delete nodes;
		nodes = next;
	}
}

template <typename T>
void lock_free_queue<T>::enqueue(T item) {
	node * new_node = new node(item);
	while (true) {
		node * cur_tail = tail.load();
		node * cur_tail_next = cur_tail->next;
		if (cur_tail_next == nullptr) {
			if ((tail.load()->next).compare_exchange_strong(cur_tail_next, new_node))
				break;
		}
		else {
			tail.compare_exchange_strong(cur_tail, cur_tail_next);
		}
	}
}

template <typename T>
bool lock_free_queue<T>::dequeue(T & item) {
	while (true) {
		threads_in_deque.fetch_add(1);
		node * cur_head = head.load();
		node * cur_tail = tail.load();
		node * cur_head_next = cur_head->next;
		if (cur_head == cur_tail) {
			if (!cur_head_next)
				return false;
			else
				tail.compare_exchange_strong(cur_head, cur_head_next);
		}
		else {
			if (head.compare_exchange_strong(cur_head, cur_head_next)) {
				item = cur_head_next->value;
				if (threads_in_deque.load() == 1) {
					node * to_delete = to_be_deleted.load();
					if (to_be_deleted.compare_exchange_strong(to_delete, cur_head_next)) {
							delete_nodes(to_delete, cur_head_next);
					}
				}
				threads_in_deque.fetch_sub(1);
				return true;
			}
		}
	}
}

#endif