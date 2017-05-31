#include <iostream>
#include <vector>
#include <deque>
#include <string>

template<typename Container>
Container Reversed(const Container & cont) {
	Container reversed(cont.size());
	auto rit = reversed.rbegin();
	for (auto it = cont.begin(); it != cont.end(); ++it) {
		*rit = *it;
		++rit;
	}
	return std::move(reversed);
}

int main() {
	std::string type;
	std::cin >> type;
	std::vector<int> v1;
	std::vector<std::string> v2;
	std::size_t size;
	std::cin >> size;
	if (type == "int") {
		for (std::size_t i = 0; i < size; ++i) {
			int element;
			std::cin >> element;
			v1.push_back(element);
		}
		for (const auto & x : Reversed(v1)) {
			std::cout << x << std::endl;
		}
	}
	else {
		for (std::size_t i = 0; i < size; ++i) {
			std::string element;
			std::cin >> element;
			v2.push_back(element);
		}
		for (const auto & x : Reversed(v2)) {
			std::cout << x << std::endl;
		}
	}
	return 0;
}