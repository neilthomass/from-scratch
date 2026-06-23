#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <utility>
#include <vector>
#include <cassert>
#include <iostream>


static const int INITIAL_CAPACITY = 16;
static const float MAX_LOAD = 0.875f;
static const uint8_t EMPTY = 0x80;
static const uint8_t DELETED = 0xFE;

template <typename K, typename V> class SwissTable {
public:
  SwissTable() {
    ctrl.assign(INITIAL_CAPACITY, EMPTY);
    slots.resize(INITIAL_CAPACITY);
  }

  void insert(K key, V val) {
    auto [index, h2] = hash(key);
    while (ctrl.at(index) != EMPTY && ctrl.at(index) != DELETED) {
      if (ctrl.at(index) == h2 && slots.at(index).first == key) {
        slots[index].second = val;
        return;
      }
      index = (index + 1) & (capacity - 1);
    }

    ctrl[index] = h2;
    slots[index] = {key, val};
    size_++;

    if (static_cast<float>(size_) / capacity >= MAX_LOAD) {
      rehash();
    }
  }

  void erase(K key) {
    auto [index, h2] = hash(key);

    while (ctrl.at(index) != EMPTY) {
      if (ctrl.at(index) == h2 && slots.at(index).first == key) {
        ctrl.at(index) = DELETED;
        size_--;
        return;
      }
      index = (index + 1) & (capacity - 1);
    }
  }

  std::optional<V> find(K key) {
    auto [index, h2] = hash(key);

    while (ctrl.at(index) != EMPTY) {
      if (ctrl.at(index) == h2 && slots.at(index).first == key) {
        return slots.at(index).second;
      }
      index = (index + 1) & (capacity - 1);
    }
    return std::nullopt;
  }

  // Keep in mind &, this fucntion returns a refrence, so it can be overwritten
  V &operator[](const K &key) {
    auto [index, h2] = hash(key);

    while (ctrl[index] != EMPTY) {
      if (ctrl[index] == h2 && slots[index].first == key)
        return slots[index].second; // found
      index = (index + 1) & (ctrl.size() - 1);
    }

    if (static_cast<float>(size_) / ctrl.size() >= MAX_LOAD) {
      rehash();
      // recompute index after rehash since capacity changed
      index = (std::hash<K>{}(key) >> 7) & (ctrl.size() - 1);
      while (ctrl[index] != EMPTY)
        index = (index + 1) & (ctrl.size() - 1);
    }

    ctrl[index] = h2;
    slots[index] = {key, V{}}; // default construct V
    size_++;
    return slots[index].second;
  }

  void clear() {
    ctrl.assign(capacity, EMPTY);
    slots.clear();
    slots.resize(capacity);
    size_ = 0;
  }

  int size() { return size_; };

private:
  std::pair<size_t, size_t> hash(K key) {
    // A size_t is 64 bits, h1 is first 57 bits, h2 is the other 7 bits
    // because the size of our backing array will always be a power of two we
    // can do a trick to compute cheap mod
    // h1 mod cap
    size_t hash = std::hash<K>{}(key);
    size_t index = hash >> 7 & (capacity - 1);
    size_t h2 = hash & 0x7F;
    return {index, h2};
  }
  void rehash() {
    std::vector<uint8_t> oldCtrl = ctrl;
    std::vector<std::pair<K, V>> oldSlots = slots;
    int oldCapacity = capacity;
    capacity = capacity * 2;
    size_ = 0;

    slots.assign(capacity, {K{}, V{}});
    ctrl.assign(capacity, EMPTY);

    for (int i = 0; i < oldCapacity; i++) {
      if (oldCtrl[i] != EMPTY && oldCtrl[i] != DELETED) {
        insert(oldSlots[i].first, oldSlots[i].second);
      }
    }
  }

  int size_ = 0;
  std::vector<std::pair<K, V>> slots;
  std::vector<uint8_t> ctrl;
  size_t capacity = INITIAL_CAPACITY;
};


int main() {
  SwissTable<std::string, int> t;

  t.insert("nick", 1);
  t.insert("neil", 2);
  assert(t.find("nick") == 1);
  assert(t.find("neil") == 2);
  assert(t.find("noel") == std::nullopt);

  t.insert("nick", 99);
  assert(t.find("nick") == 99);

  t.erase("neil");
  assert(t.find("neil") == std::nullopt);
  assert(t.size() == 1);

  t["nathan"] = 42;
  assert(t["nathan"] == 42);
  t["nathan"]++;
  assert(t["nathan"] == 43);

  for (int i = 0; i < 20; i++)
    t.insert(std::to_string(i), i);
  for (int i = 0; i < 20; i++)
    assert(t.find(std::to_string(i)) == i);

  t.clear();
  assert(t.size() == 0);
  assert(t.find("nick") == std::nullopt);

  std::cout << "all tests passed\n";
}