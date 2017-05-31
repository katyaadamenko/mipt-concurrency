#include <iostream>
#include "striped_hash_set.h"

int main() {
	striped_hash_set<int> h(2);
	for (int i = 0; i < 5; ++i) {
		h.add(i);
		//if (i == 4)
			//std::cout << h.get_stripe_index(std::hash<int>()(4)) << std::endl;
	}
	//std::cout << h.get_stripe_index(std::hash<int>()(4)) << std::endl;
	for (int i = 0; i < 5; ++i)
		std::cout << (h.contains(i) ? "Yes!!!" : "No:(") << std::endl;
	std::cout << (h.contains(0) ? "!!!" : ":(") << std::endl;
	h.remove(100);
	std::cout << (h.contains(100) ? "No" : "Yes") << std::endl;
}