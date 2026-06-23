#include <memory>
#include <optional>

static constexpr size_t CACHE_LINE = 64;

struct alignas(CACHE_LINE) PaddedIndex {
  // To make fully thread safe, make this variable std::atomic<>
  size_t value;
  char pad[CACHE_LINE - sizeof(size_t)];
};

template <typename T> class RingBuffer {

public:
  RingBuffer(size_t cap)
      : capacity(cap), storage(std::make_unique<T[]>(capacity)), start{0},
        end{0} {}

  bool write(T item) {
    if (is_full())
      return false;
    storage[end.value & (capacity - 1)] = item;
    end.value++;
    return true;
  }

  std::optional<T> read() {
    if (is_empty())
      return std::nullopt;
    T item = storage[start.value & (capacity - 1)];
    start.value++;
    return item;
  }

  std::optional<T> peek() const {
    if (is_empty())
      return std::nullopt;
    return storage[start.value & (capacity - 1)];
  }

  bool is_empty() const { return end.value == start.value; }
  bool is_full() const { return end.value - start.value == capacity; }

private:
  size_t capacity;
  PaddedIndex start;
  PaddedIndex end;
  std::unique_ptr<T[]> storage;
};
