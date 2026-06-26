#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>

template <typename T, typename Compare = std::less<T>>
class Heap {
  private:
    T* data_ = nullptr;
    size_t size_ = 0;
    size_t cap_ = 0;
    Compare comp_;

    void resize(size_t newCap) {
      T* newData = new T[newCap];
      for (size_t i = 0; i < size_; ++i){
        newData[i] = data_[i];
      }
      delete[] data_;
      data_ = newData;
      cap_ = newCap;
    }

    void siftUp(size_t index) {
      while (index > 0) {
        size_t parent = (index - 1) / 2;
        if (!comp_(data_[index], data_[parent])) break;
        std::swap(data_[parent], data_[index]);
        index = parent;
      } 
    }

    void siftDown(size_t index) {
      while (true) {
        size_t left = 2 * index + 1;
        size_t right = 2 * index + 2;
        size_t largest = index;
  
        if (left < size_ && comp_(data_[left], data_[largest])) largest = left;
        if (right < size_ && comp_(data_[right], data_[largest])) largest = right;
  
        if (largest == index) break;
        std::swap(data_[index], data_[largest]);
        index = largest;
      }
    }

    public:
        Heap() = default;
        Heap(size_t cap) : data_(new T[cap]), cap_(cap) {}
        ~Heap() {delete[] data_; }

        size_t size() const { return size_; }
        bool empty() const { return size_ == 0; }

        const T& top() const {
          if (empty()) { throw std::runtime_error("Empty"); }
          return data_[0];
        }

        void push(const T& value) {
          if (size() == cap_) { cap_ == 0 ? resize(1) : resize(2 * size_); }
          data_[size_] =  value;
          siftUp(size_);
          ++size_;
        }

        void pop() {
          if (size_ == 0 ) { throw std::runtime_error("Heap is empty"); }
          data_[0] = data_[size_ - 1];
          --size_;
          siftDown(0);
        }
        void clear() {
          size_ = 0;
        }
};
