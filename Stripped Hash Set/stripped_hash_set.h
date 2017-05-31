#ifndef STRIPED_HASH_SET
#define STRIPED_HASH_SET

#include <functional>
#include <forward_list>
#include <vector>
//#include <shared_mutex>
#include <mutex>
#include <atomic>
#include <algorithm>

template <class T, class Hash = std::hash<T>>
class striped_hash_set {
public:
	explicit striped_hash_set(size_t num_stripes, size_t growth_factor = 2, 
		size_t load_factor = 2) : growth_factor(growth_factor), load_factor(load_factor), 
		stripes_mtx(num_stripes), buckets(num_stripes * 2) {}
	void add(const T & v);
	void remove(const T & v);
	bool contains(const T & v) const;
private:
	const size_t growth_factor;
	const size_t load_factor;
	std::atomic<size_t> size{ 0 };
	//std::vector<std::shared_mutex> stripes_mtx;
	std::vector<std::mutex> stripes_mtx;
	std::vector<std::forward_list<T>> buckets;
	size_t get_bucket_index(size_t hash_value)const {
		return hash_value % buckets.size();
	}
	size_t get_stripe_index(size_t hash_value)const {
		return get_bucket_index(hash_value) % stripes_mtx.size();
	}
	void expand_set(size_t num_buckets);
	static Hash hash;
};

template <class T, class Hash = std::hash<T>>
Hash striped_hash_set::hash;

template <class T, class Hash = std::hash<T>>
void striped_hash_set<T, Hash>::add(const T & v) {
	auto h = hash(v);
	std::unique_lock<std::mutex> lck(stripes_mtx[get_stripe_index(h)]);
	buckets[get_bucket_index(h)].push_front(v);
	size.fetch_add(1);
	if ((float)size / buckets.size()) {
		lck.unlock();
		expand_set(buckets.size());
	}
}

template <class T, class Hash = std::hash<T>>
void striped_hash_set<T, Hash>::expand_set(size_t num_buckets) {
	std::vector<std::unique_lock> locks;
	locks.emplace_back(std::unique_lock<std::mutex>(stripes_mtx[0]));
	if (num_buckets != buckets.size)
		return;
	for (size_t i = 1; i < stripes_mtx.size(); ++i)
		locks.emplace_back(std::unique_lock<std::mutex>(stripes_mtx[i]));
	std::vector<std::forward_list<T>> new_buckets(growth_factor * buckets.size());
	for (auto it = buckets.begin(); it != buckets.end(); ++it) {
		for (auto it_in_bucket = it->begin(); it_in_bucket != it->end(); ++it_in_bucket) {
			auto h = hash(*it_in_bucket);
			new_buckets[get_bucket_index(h)].push_front(*it_in_bucket);
		}
	}
	buckets.swap(new_buckets);
	return;
}

template <class T, class Hash = std::hash<T>>
void striped_hash_set<T, Hash>::remove(const T & v) {
	auto h = hash(v);
	//std::vector<std::unique_lock<std::shared_mutex>> lock(stripes_mtx[get_stripe_index(h)]);
	std::vector<std::unique_lock<std::mutex>> lock(stripes_mtx[get_stripe_index(h)]);
	buckets[get_bucket_index(h)].remove(v);
}

template <class T, class Hash = std::hash<T>>
bool striped_hash_set<T, Hash>::contains(const T & v)const {
	auto h = hash(v);
	//std::shared_lock<std::shared_mutex> lock(stripes_mtx[get_stripe_index(h)]);
	std::unique_lock<std::mutex> lock(stripes_mtx[get_stripe_index(h)]);
	if (std::find(buckets[get_bucket_index(h)].begin(), buckets[get_bucket_index(h)].end(), v) == buckets[get_bucket_index(h)].end())
		return false;
	return true;
}

#endif