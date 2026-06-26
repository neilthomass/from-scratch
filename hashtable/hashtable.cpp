#include <cstddef>
#include <functional>
#include <stdexcept>

template <typename K, typename V> class HashTable {
private:
  struct Entry {
    K key;
    V value;
    Entry *next = nullptr;
  };

  Entry **buckets_ = nullptr;
  size_t bucketCount_ = 16;
  size_t size_ = 0;
  double maxLoadFactor_ = 0.75;

  size_t bucketIndex(const K &key) const {
    return std::hash<K>{}(key) % bucketCount_;
  }
  void rehash() {
    size_t newBucketCount = bucketCount_ * 2;
    Entry **newBuckets = new Entry *[newBucketCount]();
    for (size_t i = 0; i < bucketCount_; i++) {
      Entry *curr = buckets_[i];
      while (curr != nullptr) {
        Entry *next = curr->next;

        size_t newIndex = std::hash<K>{}(curr->key) % newBucketCount;
        curr->next = newBuckets[newIndex];
        newBuckets[newIndex] = curr;

        curr = next;
      }
    }

    delete[] buckets_;
    buckets_ = newBuckets;
    bucketCount_ = newBucketCount;
  }

public:
  HashTable() : buckets_(new Entry *[bucketCount_]()) {}
  ~HashTable() {
    for (size_t i = 0; i < bucketCount_; i++) {
      Entry *curr = buckets_[i];
      while (curr != nullptr) {
        Entry *next = curr->next;
        delete curr;
        curr = next;
      }
    }
    delete[] buckets_;
  }

  size_t size() const { return size_; }
  bool empty() const { return size_ == 0; }
  double loadFactor() const {
    return static_cast<double>(size_) / bucketCount_;
  }

  bool contains(const K &key) const {
    size_t index = bucketIndex(key);
    Entry *curr = buckets_[index];
    while (curr != nullptr) {
      if (curr->key == key)
        return true;
      curr = curr->next;
    }
    return false;
  }

  V &at(const K &key) {
    V *result = find(key);
    if (result == nullptr)
      throw std::runtime_error("value absent");
    return *result;
  };

  V *find(const K &key) {
    size_t index = bucketIndex(key);
    Entry *curr = buckets_[index];
    while (curr != nullptr) {
      if (curr->key == key)
        return &curr->value;
      curr = curr->next;
    }
    return nullptr;
  }

  void insert(const K &key, const V &value) {
    size_t bucket = bucketIndex(key);
    buckets_[bucket] = new Entry(key, value, buckets_[bucket]);
    ++size_;
    if (loadFactor() >= maxLoadFactor_)
      rehash();
  }

  V &operator[](const K &key) {
    V *val = find(key);
    if (val == nullptr) {
      insert(key, V{});
      val = find(key);
    }
    return *val;
  }

  bool erase(const K &key) {

    size_t index = bucketIndex(key);
    Entry *curr = buckets_[index];
    Entry *prev = nullptr;

    while (curr != nullptr) {
      if (curr->key == key) {
        if (prev == nullptr)
          buckets_[index] = curr->next;
        else
          prev->next = curr->next;
        delete curr;
        size_--;
        return true;
      }
      prev = curr;
      curr = curr->next;
    }
    return false;
  }
  void clear() {

    for (size_t i = 0; i < bucketCount_; i++) {
      Entry *curr = buckets_[i];
      while (curr != nullptr) {
        Entry *next = curr->next;
        delete curr;
        curr = next;
      }
    }
  }
};
